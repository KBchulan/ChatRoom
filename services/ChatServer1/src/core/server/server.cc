#include "server.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <core/io/io.hpp>
#include <core/session/session.hpp>
#include <tools/Logger.hpp>
#include <unordered_map>

namespace core
{

struct Server::_impl
{
  boost::asio::io_context& _ioc;
  unsigned short _port;
  boost::asio::ip::tcp::acceptor _acceptor;

  std::mutex _mutex;
  std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;

  std::weak_ptr<Server> _self;

  void start(const std::shared_ptr<Server>& self)
  {
    _self = self;
    tools::Logger::getInstance().info("ChatServer1 is starting on port {}", _port);
    boost::asio::co_spawn(_ioc, start_accept(), boost::asio::detached);
  }

  boost::asio::awaitable<void> start_accept()
  {
    while (true)
    {
      // 从池子里获取一个io_context
      auto& client_ioc = IO::GetInstance().GetIOContext();

      // 异步挂起，等待新的连接
      auto socket = co_await _acceptor.async_accept(client_ioc, boost::asio::use_awaitable);
      auto eip = socket.remote_endpoint().address().to_string();

      // 启动一个新会话来处理连接
      auto session = Session::Create(std::move(socket), _self);
      session->Start();

      {
        std::lock_guard lock{_mutex};
        _sessions[session->GetUuid()] = session;
      }

      tools::Logger::getInstance().info("New connection from {}, {}", eip, session->GetUuid());
    }
  }

  _impl(boost::asio::io_context& ioc, unsigned short port)
      : _ioc(ioc), _port(port), _acceptor(_ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port))
  {
  }

  ~_impl()
  {
    tools::Logger::getInstance().info("ChatServer1 is shutting down");

    std::lock_guard lock{_mutex};
    for (auto& [uuid, session] : _sessions)
    {
      session->Stop();
    }
    _sessions.clear();
  }
};

Server::Server(boost::asio::io_context& ioc, unsigned short port) : _pimpl(std::make_unique<_impl>(ioc, port))
{
}

Server::~Server() = default;

void Server::RemoveSession(const std::string& uuid)
{
  std::lock_guard lock{_pimpl->_mutex};

  if (auto iter = _pimpl->_sessions.find(uuid); iter != _pimpl->_sessions.end())
  {
    iter->second->Stop();
    _pimpl->_sessions.erase(iter);
  }
}

void Server::Start()
{
  _pimpl->start(shared_from_this());
}

}  // namespace core