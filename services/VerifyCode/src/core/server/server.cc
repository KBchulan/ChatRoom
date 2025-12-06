#include "server.hpp"

#include <global/Global.hpp>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>

namespace core
{

VerifyCodeServiceImpl::VerifyCodeServiceImpl(std::unique_ptr<SmtpClient> smtp_client)
    : _smtp_client(std::move(smtp_client)),
      _redis_client(std::make_unique<utils::RedisClient>(
          global::server::REDIS_SERVER_HOST, global::server::REDIS_SERVER_PORT, global::server::REDIS_SERVER_PASSWORD))
{
}

grpc::Status VerifyCodeServiceImpl::VerifyCode([[maybe_unused]] grpc::ServerContext* ctx,
                                               const VerifyCodeRequest* request, VerifyCodeResponse* response)
{
  const std::string& email = request->email();
  if (email.empty())
  {
    tools::Logger::getInstance().error("Received empty email in SendVerifyCode request");
    return {grpc::StatusCode::INVALID_ARGUMENT, "email is empty"};
  }

  // 生成验证码，取前六位即可
  auto verify_code_gen = tools::SnowflakeIdGenerator::generateIdString();
  if (!verify_code_gen.has_value())
  {
    tools::Logger::getInstance().error("Failed to generate verify code");
    return {grpc::StatusCode::INTERNAL, "failed to generate verify code"};
  }

  auto verify_code = verify_code_gen.value().substr(0, 6);

  // 发送验证码
  auto send_result = _smtp_client->SendVerifyCode(email, verify_code);
  if (send_result != "")
  {
    tools::Logger::getInstance().error("Failed to send verify code to {}: {}", email, send_result);
    return {grpc::StatusCode::INTERNAL, send_result};
  }

  // 存入 Redis，设置 1 分钟过期
  auto result = _redis_client->Set(global::server::VERIFY_CODE_REDIS_KEY_PREFIX + email, verify_code,
                                   global::server::VERIFY_CODE_TTL);
  if (result != "")
  {
    tools::Logger::getInstance().error("Failed to store verify code in Redis for {}: {}", email, result);
    return {grpc::StatusCode::INTERNAL, result};
  }

  response->set_code(0);
  response->set_message("verify_code has been sent successful");
  response->mutable_data()->insert({"verify_code", verify_code});

  tools::Logger::getInstance().info("Successfully sent verify code to {}", email);
  return grpc::Status::OK;
}

}  // namespace core