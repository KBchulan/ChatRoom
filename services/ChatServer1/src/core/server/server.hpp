/******************************************************************************
 *
 * @file       server.hpp
 * @brief      聊天服务器入口，多 Acceptor 架构监听连接
 *
 * @author     KBchulan
 * @date       2025/12/13
 * @history
 ******************************************************************************/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <core/CoreExport.hpp>
#include <memory>

namespace core
{

class CORE_EXPORT Server : public std::enable_shared_from_this<Server>
{
public:
  explicit Server(unsigned short port);
  ~Server();

  void Start();
  void RemoveSession(const std::string& uuid);

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // SERVER_HPP