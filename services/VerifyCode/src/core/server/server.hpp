/******************************************************************************
 *
 * @file       server.hpp
 * @brief      邮件发送的 RPC 服务定义
 *
 * @author     KBchulan
 * @date       2025/12/05
 * @history
 ******************************************************************************/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>

#include <core/smtp/smtp.hpp>
#include <memory>
#include <utils/redis/redis.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/gen/verify_code.grpc.pb.h>
#include <utils/gen/verify_code.pb.h>
#pragma GCC diagnostic pop

namespace core
{

using namespace KBchulan::ChatRoom::VerifyCode;

class VerifyCodeServiceImpl final : public VerifyCodeService::Service
{
public:
  explicit VerifyCodeServiceImpl(std::unique_ptr<SmtpClient> smtp_client);

  grpc::Status VerifyCode(grpc::ServerContext* ctx, const VerifyCodeRequest* request,
                          VerifyCodeResponse* response) override;

private:
  std::unique_ptr<SmtpClient> _smtp_client;
  std::unique_ptr<utils::RedisClient> _redis_client;
};

}  // namespace core

#endif  // SERVER_HPP