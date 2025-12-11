#include "user_service.hpp"

#include <core/domain/do/user/user_do.hpp>
#include <core/repository/user/user_repository.hpp>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>
#include <utils/common/code.hpp>
#include <utils/common/func.hpp>
#include <utils/grpc/client/verify_code_client.hpp>
#include <utils/pool/redis/redis_pool.hpp>

namespace core
{

struct UserService::_impl
{
  tools::Logger& logger = tools::Logger::getInstance();
  utils::RedisPool& redis_pool = utils::RedisPool::GetInstance();
  UserRepository& _user_repository = UserRepository::GetInstance();
  utils::VerifyCodeClient& _verify_code_client = utils::VerifyCodeClient::GetInstance();

  void handle_send_code_request(const utils::Context& ctx, const UserSendCodeDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 检查是否已经发送
    std::string email_key = global::server::VERIFY_CODE_PREFIX + dto.email;
    auto conn = redis_pool.GetConnection();
    auto reply = conn.Exists(email_key);

    if (!reply.IsValid() || reply.IsError())
    {
      logger.error("{}: Redis error checking verify code", request_id);
      common_vo.code = utils::REDIS_ERROR;
      common_vo.message = "Redis error checking verify code";
      common_vo.data = "";
      return;
    }

    auto exists = reply.AsInteger();
    if (exists.has_value() && exists.value() > 0)
    {
      logger.error("{}: Verification code already sent to {}", request_id, dto.email);
      common_vo.code = utils::CODE_ALREADY_SENT;
      common_vo.message = "Verification code already sent";
      common_vo.data = "";
      return;
    }

    // 发送验证码
    auto result = _verify_code_client.SendVerifyCode(dto.email);

    // 发送失败
    if (!result)
    {
      logger.error("{}: Failed to send verification code: {}", request_id, result.error().message);
      common_vo.code = utils::SEND_EMAIL_CODE_FAILED;
      common_vo.message = "Failed to send verification code: " + result.error().message;
      common_vo.data = "";
      return;
    }

    // 发送成功时落盘
    UserVerifyCodeDO user_verify_code_do;
    user_verify_code_do.email = dto.email;
    user_verify_code_do.code = result->data().find("verify_code")->second;
    user_verify_code_do.purpose = dto.purpose;

    if (!_user_repository.InsertVerifyCode(user_verify_code_do))
    {
      logger.error("{}: Failed to insert verification code into database.", request_id);
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = "Failed to insert verification code into database.";
      common_vo.data = "";
      return;
    }
  }

  void handle_register_request(const utils::Context& ctx, const UserRegisterDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 检查用户是否已注册
    auto user_exists = _user_repository.CheckUserExists(dto.email, dto.nickname);
    if (!user_exists.has_value())
    {
      logger.error("{}: Error checking user existence: {}", request_id, user_exists.error());
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = user_exists.error();
      common_vo.data = "";
      return;
    }
    if (user_exists.value())
    {
      logger.error("{}: User with email {} or nickname {} already exists", request_id, dto.email, dto.nickname);
      common_vo.code = utils::USER_ALREADY_EXISTS;
      common_vo.message = "User with given email or nickname already exists";
      common_vo.data = "";
      return;
    }

    // 校验验证码
    std::string email_key = global::server::VERIFY_CODE_PREFIX + dto.email;
    auto conn = redis_pool.GetConnection();
    auto reply = conn.Get(email_key);

    if (!reply.IsValid() || reply.IsError())
    {
      logger.error("{}: Redis error checking verify code", request_id);
      common_vo.code = utils::REDIS_ERROR;
      common_vo.message = "Redis error checking verify code";
      common_vo.data = "";
      return;
    }
    if (reply.IsNil())
    {
      logger.error("{}: Verification code expired for {}", request_id, dto.email);
      common_vo.code = utils::EXPIRED_VERIFY_CODE;
      common_vo.message = "Verification code expired";
      common_vo.data = "";
      return;
    }
    if (dto.verify_code != reply.AsString())
    {
      logger.error("{}: Error verification code for {}", request_id, dto.email);
      common_vo.code = utils::ERROR_VERIFY_CODE;
      common_vo.message = "Error verification code";
      common_vo.data = "";
      return;
    }

    // 加密密码
    auto hash_pass = utils::HashPassWord(dto.password);
    if (!hash_pass.has_value())
    {
      logger.error("{}: Error in hash password: {}", request_id, dto.password);
      common_vo.code = utils::ERROR_HASH_PASSWORD;
      common_vo.message = "Error in hash password";
      common_vo.data = "";
      return;
    }

    // 插入注册数据
    UserDO user_do;

    auto uuid = tools::UuidGenerator::generateUuid();
    if (!uuid.has_value())
    {
      logger.error("{}: Failed to generate uuid for user", request_id);
      common_vo.code = utils::UUID_GENERATION_FAILED;
      common_vo.message = "Failed to generate uuid for user";
      common_vo.data = "";
      return;
    }

    user_do.uuid = uuid.value();
    user_do.nickname = dto.nickname;
    user_do.avatar = global::server::DEFAULT_AVATAR_URL;
    user_do.email = dto.email;
    user_do.password_hash = hash_pass.value();

    if (!_user_repository.InsertUser(user_do))
    {
      logger.error("{}: Error in insert user to database", request_id);
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = "Error in insert user to database";
      common_vo.data = "";
      return;
    }

    // 清除 redis 中的验证码
    reply = conn.Del(email_key);
    if (!reply.IsValid() || reply.IsError() || reply.AsInteger() == 0)
    {
      logger.warning("{}: Redis error delete verify code", request_id);
    }
  }

  void handle_reset_request(const utils::Context& ctx, const UserResetPasswordDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 检查用户是否存在
    auto user_exists = _user_repository.CheckUserExists(dto.email, "");
    if (!user_exists.has_value())
    {
      logger.error("{}: Error checking user existence: {}", request_id, user_exists.error());
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = user_exists.error();
      common_vo.data = "";
      return;
    }
    if (!user_exists.value())
    {
      logger.error("{}: User with email {} not exists", request_id, dto.email);
      common_vo.code = utils::USER_NOT_EXISTS;
      common_vo.message = "User with given email or nickname not exists";
      common_vo.data = "";
      return;
    }

    // 校验验证码
    std::string email_key = global::server::VERIFY_CODE_PREFIX + dto.email;
    auto conn = redis_pool.GetConnection();
    auto reply = conn.Get(email_key);

    if (!reply.IsValid() || reply.IsError())
    {
      logger.error("{}: Redis error checking verify code", request_id);
      common_vo.code = utils::REDIS_ERROR;
      common_vo.message = "Redis error checking verify code";
      common_vo.data = "";
      return;
    }
    if (reply.IsNil())
    {
      logger.error("{}: Verification code expired for {}", request_id, dto.email);
      common_vo.code = utils::EXPIRED_VERIFY_CODE;
      common_vo.message = "Verification code expired";
      common_vo.data = "";
      return;
    }
    if (dto.verify_code != reply.AsString())
    {
      logger.error("{}: Error verification code for {}", request_id, dto.email);
      common_vo.code = utils::ERROR_VERIFY_CODE;
      common_vo.message = "Error verification code";
      common_vo.data = "";
      return;
    }

    // 加密密码
    auto hash_pass = utils::HashPassWord(dto.password);
    if (!hash_pass.has_value())
    {
      logger.error("{}: Error in hash password: {}", request_id, dto.password);
      common_vo.code = utils::ERROR_HASH_PASSWORD;
      common_vo.message = "Error in hash password";
      common_vo.data = "";
      return;
    }

    // 更新用户密码
    if (!_user_repository.UpdateUserPassword(dto.email, hash_pass.value()))
    {
      logger.error("{}: Error in update user password to database", request_id);
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = "Error in update user password to database";
      common_vo.data = "";
      return;
    }

    // 清除 redis 中的验证码
    reply = conn.Del(email_key);
    if (!reply.IsValid() || reply.IsError() || reply.AsInteger() == 0)
    {
      logger.warning("{}: Redis error delete verify code", request_id);
    }
  }

  _impl() = default;
  ~_impl() = default;
};

UserService::UserService() : _pimpl(std::make_unique<_impl>())
{
}

UserService::~UserService() = default;

UserService& UserService::GetInstance()
{
  static UserService instance;
  return instance;
}

void UserService::HandleSendCodeRequest(const utils::Context& ctx, const UserSendCodeDTO& dto,
                                        core::CommonVO& common_vo) const
{
  _pimpl->handle_send_code_request(ctx, dto, common_vo);
}

void UserService::HandleRegisterRequest(const utils::Context& ctx, const UserRegisterDTO& dto,
                                        core::CommonVO& common_vo) const
{
  _pimpl->handle_register_request(ctx, dto, common_vo);
}

void UserService::HandleResetRequest(const utils::Context& ctx, const UserResetPasswordDTO& dto,
                                     core::CommonVO& common_vo) const
{
  _pimpl->handle_reset_request(ctx, dto, common_vo);
}

}  // namespace core