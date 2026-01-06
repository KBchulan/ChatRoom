/******************************************************************************
 *
 * @file       status_server_client.hpp
 * @brief      与状态服务器通信的客户端
 *
 * @author     KBchulan
 * @date       2025/12/15
 * @history
 ******************************************************************************/

#ifndef STATUS_SERVER_CLIENT_HPP
#define STATUS_SERVER_CLIENT_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/grpc/status_server/status_server.grpc.pb.h>
#include <utils/grpc/status_server/status_server.pb.h>
#pragma GCC diagnostic pop

#include <expected>
#include <utils/UtilsExport.hpp>
#include <utils/grpc/client/grpc_error.hpp>
#include <utils/pool/channel/channel_pool.hpp>

namespace utils
{

using namespace KBchulan::ChatRoom::StatusServer;

using LoginVerifyResult = std::expected<LoginVerifyResponse, GrpcError>;

class UTILS_EXPORT StatusServerClinet
{
public:
  static StatusServerClinet& GetInstance();

  void Init(const std::string& server_address, std::size_t pool_size);

  [[nodiscard]] LoginVerifyResult VerifyLoginInfo(const std::string& uuid, const std::string& token);

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

#endif  // STATUS_SERVER_CLIENT_HPP