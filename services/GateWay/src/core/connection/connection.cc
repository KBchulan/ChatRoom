#include "connection.hpp"

#include <boost/asio/steady_timer.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/verb.hpp>
#include <core/business/business.hpp>
#include <core/logic/logic.hpp>
#include <global/Global.hpp>
#include <tools/Defer.hpp>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>
#include <utils/common/type.hpp>
#include <utils/context/context.hpp>

namespace core
{

struct Connection::_impl
{
  boost::asio::ip::tcp::socket _socket;
  boost::beast::flat_buffer _buffer{global::server::MAX_FLATBUFFER_SIZE};
  boost::beast::http::request<boost::beast::http::dynamic_body> _request;
  boost::beast::http::response<boost::beast::http::dynamic_body> _response;
  boost::asio::steady_timer _deadline;

  std::function<void(const boost::beast::error_code&, std::size_t)> _request_handler;
  std::function<void(const boost::beast::error_code&, std::size_t)> _response_handler;
  std::function<void(const boost::system::error_code&)> _deadline_handler;

  std::weak_ptr<Connection> _self;

  void start(const std::shared_ptr<Connection>& self)
  {
    _self = self;

    _request_handler = [this, self](const boost::beast::error_code& errc, std::size_t) -> void
    {
      if (errc)
      {
        // 读取请求出错，记录日志并关闭连接
        tools::Logger::getInstance().error("Read request error, error msg: {}", errc.message());
        return;
      }

      // 成功读取请求，处理请求并启动定时器
      check_deadline();
      handle_request();
    };

    _response_handler = [this, self](const boost::beast::error_code& errc, std::size_t) -> void
    {
      if (errc)
      {
        // 发送响应出错，记录日志并关闭连接
        tools::Logger::getInstance().error("Send response error, error msg: {}", errc.message());
      }

      // 成功发送响应，关闭连接
      boost::beast::error_code shutdown_ec;
      _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, shutdown_ec);
      _deadline.cancel();
    };

    _deadline_handler = [this, self](const boost::system::error_code& errc) -> void
    {
      // 超时则关闭连接
      if (!errc)
      {
        boost::beast::error_code shutdown_ec;
        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_ec);
      }
    };

    boost::beast::http::async_read(_socket, _buffer, _request, std::move(_request_handler));
  }

  void check_deadline()
  {
    _deadline.async_wait(std::move(_deadline_handler));
  }

  void handle_request()
  {
    auto self = _self.lock();
    if (!self)
    {
      tools::Logger::getInstance().error("Connection already destroyed");
      return;
    }

    // 构造响应的基本信息
    _response.version(_request.version());
    _response.keep_alive(false);
    _response.set(boost::beast::http::field::content_type, "application/json");
    _response.set(boost::beast::http::field::server, "ChatRoom-GateWay");

    // 解析 URL
    auto url = utils::ParseUrl(_request.target());
    if (!url.has_value())
    {
      int status_code = static_cast<int>(boost::beast::http::status::bad_request);
      tools::Logger::getInstance().error(" {} | {} | {}", _request.method_string(), status_code, _request.target());
      _response.result(boost::beast::http::status::bad_request);
      boost::beast::ostream(_response.body()) << "Invalid URL";
      send_response();
      return;
    }

    // 构造请求上下文
    std::string body = boost::beast::buffers_to_string(_request.body().data());
    utils::Context ctx(url.value(), body);

    // 增加 request id 方便追踪日志
    auto request_id = tools::UuidGenerator::generateShortUuid();
    if (!request_id.has_value())
    {
      int status_code = static_cast<int>(boost::beast::http::status::bad_request);
      tools::Logger::getInstance().error(" {} | {} | {}", _request.method_string(), status_code, _request.target());
      _response.result(boost::beast::http::status::bad_request);
      boost::beast::ostream(_response.body()) << "Generate request ID failed";
      send_response();
      return;
    }

    ctx.Set("request_id", request_id.value());
    _response.set("X-Request-ID", request_id.value());

    // TODO: 增加重复请求检测机制/幂等性
    // TODO: 增加 JWT 校验机制

    // 将请求交给业务线程池处理
    boost::asio::post(
        Business::GetInstance().GetBusinessPool(),
        [this, self, ctx = std::move(ctx), request_id = request_id.value(), method = _request.method(),
         method_str = std::string(_request.method_string()), target = std::string(_request.target())]() mutable
        {
          // 处理请求
          auto result = [&]() -> RequestHandleResult
          {
            switch (method)
            {
              case boost::beast::http::verb::get:
                return Logic::GetInstance().HandleGetRequest(ctx);
              case boost::beast::http::verb::post:
                return Logic::GetInstance().HandlePostRequest(ctx);
              case boost::beast::http::verb::put:
                return Logic::GetInstance().HandlePutRequest(ctx);
              case boost::beast::http::verb::delete_:
                return Logic::GetInstance().HandleDeleteRequest(ctx);
              default:
                return std::unexpected("Unsupported HTTP method");
            }
          }();

          // 将结果发送回客户端，重新回到网络线程
          boost::asio::post(
              _socket.get_executor(),
              [this, self, result = std::move(result), request_id = std::move(request_id),
               method_str = std::move(method_str), target = std::move(target)]()
              {
                int status_code = 0;

                if (result.has_value())
                {
                  status_code = static_cast<int>(boost::beast::http::status::ok);
                  tools::Logger::getInstance().info(" {} | {} | {} | {}", request_id, method_str, status_code, target);
                  _response.result(boost::beast::http::status::ok);
                  boost::beast::ostream(_response.body()) << result.value();
                }
                else
                {
                  status_code = static_cast<int>(boost::beast::http::status::bad_request);
                  tools::Logger::getInstance().error(" {} | {} | {} | {}", request_id, method_str, status_code, target);
                  _response.result(boost::beast::http::status::bad_request);
                  boost::beast::ostream(_response.body()) << result.error();
                }

                send_response();
              });
        });
  }

  void send_response()
  {
    _response.prepare_payload();
    boost::beast::http::async_write(_socket, _response, std::move(_response_handler));
  }

  explicit _impl(boost::asio::ip::tcp::socket socket)
      : _socket(std::move(socket)), _deadline(_socket.get_executor(), std::chrono::seconds(60))
  {
  }
};

Connection::Connection(boost::asio::ip::tcp::socket socket) : _pimpl(std::make_unique<_impl>(std::move(socket)))
{
}

Connection::~Connection() = default;

void Connection::Start()
{
  _pimpl->start(shared_from_this());
}

}  // namespace core