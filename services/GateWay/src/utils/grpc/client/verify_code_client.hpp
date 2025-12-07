/******************************************************************************
 *
 * @file       verify_code_client.hpp
 * @brief      发送验证码的 rpc 客户端
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef VERIFY_CODE_CLIENT_HPP
#define VERIFY_CODE_CLIENT_HPP

#include <grpcpp/support/status.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/grpc/verify_code/verify_code.grpc.pb.h>
#include <utils/grpc/verify_code/verify_code.pb.h>
#pragma GCC diagnostic pop

#include <cstddef>
#include <expected>
#include <memory>
#include <utils/UtilsExport.hpp>
#include <utils/pool/channel/channel_pool.hpp>

namespace utils
{

using namespace KBchulan::ChatRoom::VerifyCode;

struct UTILS_EXPORT GrpcError
{
  grpc::StatusCode code;
  std::string message;
};

using VerifyCodeResult = std::expected<VerifyCodeResponse, GrpcError>;

class UTILS_EXPORT VerifyCodeClient
{
public:
  static VerifyCodeClient& GetInstance();

  void Init(const std::string& server_address, std::size_t pool_size);

  [[nodiscard]] VerifyCodeResult SendVerifyCode(std::string_view email);

  VerifyCodeClient(const VerifyCodeClient&) = delete;
  VerifyCodeClient& operator=(const VerifyCodeClient&) = delete;
  VerifyCodeClient(VerifyCodeClient&&) = delete;
  VerifyCodeClient& operator=(VerifyCodeClient&&) = delete;

private:
  VerifyCodeClient() = default;
  ~VerifyCodeClient() = default;

  [[nodiscard]] std::unique_ptr<VerifyCodeService::Stub> create_stub();

  std::unique_ptr<ChannelPool> _pool;
};

}  // namespace utils

#endif  // VERIFY_CODE_CLIENT_HPP