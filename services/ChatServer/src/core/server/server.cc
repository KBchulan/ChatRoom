#include "server.hpp"

#include <sys/socket.h>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/system/detail/error_code.hpp>
#include <core/io/io.hpp>
#include <core/session/session.hpp>
#include <functional>
#include <tools/Logger.hpp>
#include <unordered_map>

namespace core
{

// 单个 Acceptor 工作单元，每个 io_context 对应一个，使用协程风格
class AcceptorWorker
{
public:
  using SessionCallback = std::function<void(std::shared_ptr<Session>)>;

  AcceptorWorker(boost::asio::io_context& ioc, unsigned short port, std::weak_ptr<Server> server,
                 SessionCallback on_session)
      : _io_context(ioc), _acceptor(ioc), _server(std::move(server)), _on_session(std::move(on_session))
  {
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::socket_base::reuse_address(true));

    // SO_REUSEPORT: 允许多个 socket 绑定同一端口，内核级负载均衡
    int reuse_port = 1;
    setsockopt(_acceptor.native_handle(), SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));

    _acceptor.bind(endpoint);
    _acceptor.listen(boost::asio::socket_base::max_listen_connections);

    // 启动协程 accept 循环
    boost::asio::co_spawn(_io_context, accept_loop(), boost::asio::detached);
  }

  ~AcceptorWorker()
  {
    boost::system::error_code errc;
    _acceptor.close(errc);
  }

private:
  boost::asio::awaitable<void> accept_loop()
  {
    while (_acceptor.is_open())
    {
      try
      {
        // 从池子里获取一个 io_context 给新连接使用
        auto& client_ioc = IO::GetInstance().GetIOContext();

        // 异步挂起，等待新的连接
        auto socket = co_await _acceptor.async_accept(client_ioc, boost::asio::use_awaitable);
        auto eip = socket.remote_endpoint().address().to_string();

        // 启动一个新会话来处理连接
        auto session = Session::Create(std::move(socket), _server);
        session->Start();

        // 通过回调注册到 Server 的 session 管理
        if (_on_session)
        {
          _on_session(session);
        }

        tools::Logger::getInstance().info("New connection from {}, {}", eip, session->GetUuid());
      }
      catch (const boost::system::system_error& e)
      {
        if (e.code() == boost::asio::error::operation_aborted)
        {
          break;
        }
        tools::Logger::getInstance().error("Accept error: {}", e.what());
      }
    }
  }

  boost::asio::io_context& _io_context;
  boost::asio::ip::tcp::acceptor _acceptor;
  std::weak_ptr<Server> _server;
  SessionCallback _on_session;
};

struct Server::_impl
{
  unsigned short _port;
  std::vector<std::unique_ptr<AcceptorWorker>> _workers;

  std::mutex _mutex;
  std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;

  std::weak_ptr<Server> _self;

  void start(const std::shared_ptr<Server>& self)
  {
    _self = self;

    auto& io_pool = IO::GetInstance();
    auto pool_size = io_pool.GetPoolSize();

    _workers.reserve(pool_size);

    // 每个 io_context 创建一个 acceptor worker
    for (std::size_t i = 0; i < pool_size; ++i)
    {
      _workers.emplace_back(std::make_unique<AcceptorWorker>(io_pool.GetIOContextAt(i), _port, _self,
                                                             [this](std::shared_ptr<Session> session)
                                                             {
                                                               std::lock_guard lock{_mutex};
                                                               _sessions[session->GetUuid()] = std::move(session);
                                                             }));
    }

    tools::Logger::getInstance().info("ChatServer started on port {} with {} acceptors", _port, pool_size);
  }

  explicit _impl(unsigned short port) : _port(port)
  {
  }

  ~_impl()
  {
    tools::Logger::getInstance().info("ChatServer is shutting down");

    std::lock_guard lock{_mutex};
    for (auto& [uuid, session] : _sessions)
    {
      session->Stop();
    }
    _sessions.clear();
  }
};

Server::Server(unsigned short port) : _pimpl(std::make_unique<_impl>(port))
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
