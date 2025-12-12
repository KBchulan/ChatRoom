/******************************************************************************
 *
 * @file       server.hpp
 * @brief      状态服务器的 RPC 服务器端
 *
 * @author     KBchulan
 * @date       2025/12/12
 * @history
 ******************************************************************************/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <core/CoreExport.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/gen/status_server.grpc.pb.h>
#include <utils/gen/status_server.pb.h>
#pragma GCC diagnostic pop

namespace core
{

using namespace KBchulan::ChatRoom::StatusServer;

struct CORE_EXPORT TcpServerInfo
{
  std::string host;
  int port;
  int connection_count = 0;
};

class CORE_EXPORT StatusServiceImpl final : public StatusService::Service
{
public:
  StatusServiceImpl(const std::vector<TcpServerInfo>& servers) : _tcp_servers(servers)
  {
  }

  ~StatusServiceImpl() override = default;

  grpc::Status GetTcpServer(grpc::ServerContext* ctx, const GetTcpServerRequest* request,
                            GetTcpServerResponse* response) override;

private:
  std::vector<TcpServerInfo> _tcp_servers;
  std::unordered_map<std::string, std::string> _tokens;
};

}  // namespace core

#endif  // SERVER_HPP