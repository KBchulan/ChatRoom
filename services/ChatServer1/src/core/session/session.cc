#include "session.hpp"

#include <array>
#include <atomic>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <core/logic/logic.hpp>
#include <core/msg-node/msg-node.hpp>
#include <core/server/server.hpp>
#include <global/Global.hpp>
#include <global/SuperQueue.hpp>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>

namespace core
{

using namespace boost::asio::experimental::awaitable_operators;

struct Session::_impl
{
  std::atomic<bool> _closed;
  boost::asio::ip::tcp::socket _socket;

  // 用于发送的结构
  global::SuperQueue<std::shared_ptr<SendNode>, global::server::SEND_QUEUE_CAPACITY> _send_queue;
  boost::asio::steady_timer _send_notify_timer;

  // 用于接收的结构
  std::array<char, global::server::RECV_BUFFER_SIZE> _recv_buffer;

  std::string _uuid;
  std::weak_ptr<Server> _server;
  std::weak_ptr<Session> _self;

  boost::asio::awaitable<void> read_loop()
  {
    using namespace global::server;

    while (!_closed.load(std::memory_order_acquire))
    {
      co_await boost::asio::async_read(_socket, boost::asio::buffer(_recv_buffer.data(), MSG_HEAD_TOTAL_LEN),
                                       boost::asio::use_awaitable);

      short msg_id = 0;
      short msg_len = 0;

      std::memcpy(&msg_id, _recv_buffer.data(), MSG_TYPE_LENGTH);
      std::memcpy(&msg_len, _recv_buffer.data() + MSG_TYPE_LENGTH, MSG_LEN_LENGTH);

      msg_id = static_cast<short>(
          boost::asio::detail::socket_ops::network_to_host_short(static_cast<unsigned short>(msg_id)));
      msg_len = static_cast<short>(
          boost::asio::detail::socket_ops::network_to_host_short(static_cast<unsigned short>(msg_len)));

      if (msg_len <= 0 || static_cast<std::size_t>(msg_len) > RECV_BUFFER_SIZE)
      {
        tools::Logger::getInstance().error("Session received invalid msg_len: {}", msg_len);
        co_return;
      }

      co_await boost::asio::async_read(_socket,
                                       boost::asio::buffer(_recv_buffer.data(), static_cast<std::size_t>(msg_len)),
                                       boost::asio::use_awaitable);

      auto recv_node = std::make_shared<RecvNode>(
          msg_id, std::span<const char>(_recv_buffer.data(), static_cast<std::size_t>(msg_len)));
      Logic::GetInstance().PostToLogic(_self.lock(), recv_node);
    }
  }

  boost::asio::awaitable<void> write_loop()
  {
    using namespace global::server;

    while (!_closed.load(std::memory_order_acquire))
    {
      std::shared_ptr<SendNode> msg;

      // 如果队列中没有消息，启动一个定时器
      while (!_send_queue.pop(msg))
      {
        if (_closed.load(std::memory_order_acquire))
        {
          co_return;
        }

        _send_notify_timer.expires_after(std::chrono::seconds(SEND_EXPIRE_TIME_S));
        boost::system::error_code errc;
        co_await _send_notify_timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, errc));
      }

      auto data = msg->GetData();
      co_await boost::asio::async_write(_socket, boost::asio::buffer(data.data(), data.size()),
                                        boost::asio::use_awaitable);
    }
  }

  void start(const Ptr& self)
  {
    _self = self;

    boost::asio::co_spawn(
        _socket.get_executor(),
        [self, this]() -> boost::asio::awaitable<void>
        {
          try
          {
            co_await (self->_pimpl->read_loop() || self->_pimpl->write_loop());
          }
          catch (const std::exception& e)
          {
            tools::Logger::getInstance().error("Session error: {}", e.what());
          }

          if (auto server = _server.lock())
          {
            server->RemoveSession(_uuid);
          }
        },
        boost::asio::detached);
  }

  void stop()
  {
    _closed.store(true, std::memory_order_release);
    _send_notify_timer.cancel();

    boost::system::error_code errc;
    _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errc);
    _socket.close(errc);
  }

  _impl(boost::asio::ip::tcp::socket socket, const std::weak_ptr<Server>& server)
      : _closed(false),
        _socket(std::move(socket)),
        _send_notify_timer(_socket.get_executor()),
        _recv_buffer(),
        _uuid(tools::UuidGenerator::generateUuid().value()),
        _server(server)
  {
  }

  ~_impl()
  {
    tools::Logger::getInstance().info("Session {} closed", _uuid);
  }
};

Session::Ptr Session::Create(boost::asio::ip::tcp::socket socket, const std::weak_ptr<Server>& server)
{
  return Ptr{new Session(std::move(socket), server)};
}

Session::~Session() = default;

void Session::Start()
{
  _pimpl->start(shared_from_this());
}

void Session::Stop()
{
  _pimpl->stop();
}

void Session::Send(const std::shared_ptr<SendNode>& msg)
{
  if (_pimpl->_closed.load(std::memory_order_acquire))
  {
    return;
  }

  if (_pimpl->_send_queue.emplace(msg))
  {
    _pimpl->_send_notify_timer.cancel();
  }
  else
  {
    tools::Logger::getInstance().error("Session send queue is full");
  }
}

const std::string& Session::GetUuid() const
{
  return _pimpl->_uuid;
}

Session::Session(boost::asio::ip::tcp::socket socket, const std::weak_ptr<Server>& server)
    : _pimpl(std::make_unique<_impl>(std::move(socket), server))
{
}

}  // namespace core