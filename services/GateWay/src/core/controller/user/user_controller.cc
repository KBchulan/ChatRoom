#include "user_controller.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <regex>
#pragma GCC diagnostic pop

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <core/service/user/user_service.hpp>
#include <global/Global.hpp>
#include <tools/Logger.hpp>
#include <utils/common/code.hpp>
#include <utils/pool/redis/redis_pool.hpp>

namespace core
{

struct UserController::_impl
{
  UserService& user_service = UserService::GetInstance();
  tools::Logger& logger = tools::Logger::getInstance();
  utils::RedisPool& redis_pool = utils::RedisPool::GetInstance();

  [[nodiscard]] static bool is_valid_email(const std::string& email)
  {
    // 基本的邮箱正则表达式
    static const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
  }

  void handle_send_code_request(const utils::Context& ctx, const UserSendCodeDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 校验参数
    if (dto.email.empty() || (dto.purpose != 1 && dto.purpose != 2 && dto.purpose != 3))
    {
      logger.error("{}: Invalid parameters", request_id);
      common_vo.code = utils::INVALID_PARAMETERS;
      common_vo.message = "Invalid parameters";
      common_vo.data = "";
      return;
    }

    if (!is_valid_email(dto.email))
    {
      logger.error("{}: Invalid email format", request_id);
      common_vo.code = utils::INVALID_EMAIL_FORMAT;
      common_vo.message = "Invalid email format";
      common_vo.data = "";
      return;
    }

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

    // 调用 service 层发送验证码
    user_service.HandleSendCodeRequest(ctx, dto, common_vo);
    if (common_vo.code != utils::SUCCESS)
    {
      return;
    }

    common_vo.code = utils::SUCCESS;
    common_vo.message = "Verification code sent successfully";
    common_vo.data = "";
  }
};

UserController::UserController() : _pimpl(std::make_unique<_impl>())
{
}

UserController::~UserController() = default;

UserController& UserController::GetInstance()
{
  static UserController instance;
  return instance;
}

void UserController::HandleSendCodeRequest(const utils::Context& ctx, const UserSendCodeDTO& dto,
                                           core::CommonVO& common_vo) const
{
  _pimpl->handle_send_code_request(ctx, dto, common_vo);
}

}  // namespace core