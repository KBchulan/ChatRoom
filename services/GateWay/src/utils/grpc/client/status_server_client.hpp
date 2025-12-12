/******************************************************************************
 *
 * @file       tcp_server_client.hpp
 * @brief      向状态服务器获取聊天服务器的客户端
 *
 * @author     KBchulan
 * @date       2025/12/12
 * @history
 ******************************************************************************/

#ifndef TCP_SERVER_CLIENT_HPP
#define TCP_SERVER_CLIENT_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/grpc/status_server/status_server.grpc.pb.h>
#include <utils/grpc/status_server/status_server.pb.h>
#pragma GCC diagnostic pop

#include <expected>
#include <memory>
#include <utils/UtilsExport.hpp>
#include <utils/pool/channel/channel_pool.hpp>

namespace utils
{

using namespace KBchulan::ChatRoom::StatusServer;

struct GrpcError;
using GetTcpServerResult = std::expected<GetTcpServerResponse, GrpcError>;

class UTILS_EXPORT StatusServerClinet
{
public:
  static StatusServerClinet& GetInstance();

  void Init(const std::string& server_address, std::size_t pool_size);

  [[nodiscard]] GetTcpServerResult GetTcpServer(std::string_view uuid);

  StatusServerClinet(const StatusServerClinet&) = delete;
  StatusServerClinet& operator=(const StatusServerClinet&) = delete;
  StatusServerClinet(StatusServerClinet&&) = delete;
  StatusServerClinet& operator=(StatusServerClinet&&) = delete;

private:
  StatusServerClinet() = default;
  ~StatusServerClinet() = default;

  [[nodiscard]] std::unique_ptr<StatusService::Stub> create_stub();

  std::unique_ptr<ChannelPool> _pool;
};

}  // namespace utils

#endif  // TCP_SERVER_CLIENT_HPP