/******************************************************************************
 *
 * @file       session.hpp
 * @brief      单个会话层
 *
 * @author     KBchulan
 * @date       2025/12/13
 * @history
 ******************************************************************************/

#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <core/CoreExport.hpp>
#include <memory>

namespace core
{

class Server;
class SendNode;
class CORE_EXPORT Session : public std::enable_shared_from_this<Session>
{
public:
  using Ptr = std::shared_ptr<Session>;

  // 提供一个工厂方法来创建Session实例
  [[nodiscard]] static Ptr Create(boost::asio::ip::tcp::socket socket, const std::weak_ptr<Server>& server);
  ~Session();

  // 启动读写协程
  void Start();

  // 停止读写协程
  void Stop();

  // 非阻塞发送消息
  void Send(const std::shared_ptr<SendNode>& msg);

  [[nodiscard]] const std::string& GetUuid() const;

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;
  Session(Session&&) = delete;
  Session& operator=(Session&&) = delete;

private:
  Session(boost::asio::ip::tcp::socket socket, const std::weak_ptr<Server>& server);

  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // SESSION_HPP