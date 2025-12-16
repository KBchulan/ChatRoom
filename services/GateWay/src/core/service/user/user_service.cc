#include "user_service.hpp"

#include <core/domain/do/user/user_do.hpp>
#include <core/repository/user/user_repository.hpp>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>
#include <utils/common/code.hpp>
#include <utils/common/func.hpp>
#include <utils/grpc/client/status_server_client.hpp>
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
  utils::StatusServerClinet& _status_server_client = utils::StatusServerClinet::GetInstance();

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

  void handle_login_request(const utils::Context& ctx, const UserLoginDTO& dto, core::CommonVO& common_vo,
                            UserLoginVO& login_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 检查用户是否存在
    auto user_exists = _user_repository.CheckUserExists(dto.is_email ? dto.user : "", dto.is_email ? "" : dto.user);
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
      logger.error("{}: User with {} not exists", request_id, dto.user);
      common_vo.code = utils::USER_NOT_EXISTS;
      common_vo.message = "User with given info not exists";
      common_vo.data = "";
      return;
    }

    // 获取用户 uuid 和 password_hash
    auto [user_uuid, password_hash] = _user_repository.GetUidPassByUser(dto.user, dto.is_email);
    if (password_hash.empty() || user_uuid.empty())
    {
      logger.error("{}: Error getting user info by: {}", request_id, dto.user);
      common_vo.code = utils::DATABASE_ERROR;
      common_vo.message = "Error getting user by input info";
      common_vo.data = "";
      return;
    }

    // 判断用户是否已经登录
    auto conn = redis_pool.GetConnection();
    auto redis_info_exists = conn.Exists(global::server::USER_INFO_PREFIX + user_uuid);
    if (!redis_info_exists.IsValid() || redis_info_exists.IsError())
    {
      logger.error("{}: Redis error checking user info", request_id);
      common_vo.code = utils::REDIS_ERROR;
      common_vo.message = "Redis error checking user info";
      common_vo.data = "";
      return;
    }
    if (redis_info_exists.AsInteger().has_value() && redis_info_exists.AsInteger().value() > 0)
    {
      logger.error("{}: User {} already logged in", request_id, dto.user);
      common_vo.code = utils::USER_ALREADY_LOGGED_IN;
      common_vo.message = "User already logged in";
      common_vo.data = "";
      return;
    }

    // 验证密码
    auto verify_pass = utils::VerifyPassword(dto.password, password_hash);
    if (!verify_pass)
    {
      logger.error("{}: Error password for user {}", request_id, dto.user);
      common_vo.code = utils::ERROR_PASSWORD;
      common_vo.message = "Error password for user";
      common_vo.data = "";
      return;
    }

    // 调用 RPC 服务获取空闲的 Tcp server
    auto res = _status_server_client.GetTcpServer(user_uuid);
    if (!res)
    {
      logger.error("{}: Error getting Tcp server for user {}, err is: {}", request_id, user_uuid, res.error().message);
      common_vo.code = utils::ERROR_GET_TCP_SERVER;
      common_vo.message = res.error().message;
      common_vo.data = "";
      return;
    }

    login_vo.uuid = user_uuid;
    login_vo.host = res->data().find("host")->second;
    login_vo.port = static_cast<int16_t>(std::stoi(res->data().find("port")->second));
    login_vo.token = res->data().find("token")->second;
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

void UserService::HandleLoginRequest(const utils::Context& ctx, const UserLoginDTO& dto, core::CommonVO& common_vo,
                                     UserLoginVO& login_vo) const
{
  _pimpl->handle_login_request(ctx, dto, common_vo, login_vo);
}

}  // namespace core