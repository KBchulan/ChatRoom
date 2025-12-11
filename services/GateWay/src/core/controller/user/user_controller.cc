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

namespace core
{

struct UserController::_impl
{
  UserService& user_service = UserService::GetInstance();
  tools::Logger& logger = tools::Logger::getInstance();

  [[nodiscard]] static bool is_valid_email(const std::string& email)
  {
    // 基本的邮箱正则表达式
    static const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
  }

  [[nodiscard]] static bool is_valid_password(const std::string& password)
  {
    // 基本的密码正则表达式，至少6位，包含字母和数字
    static const std::regex pattern(R"((?=.*[A-Za-z])(?=.*\d)[A-Za-z\d]{6,})");
    return std::regex_match(password, pattern);
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

    // 验证邮箱格式
    if (!is_valid_email(dto.email))
    {
      logger.error("{}: Invalid email format", request_id);
      common_vo.code = utils::INVALID_EMAIL_FORMAT;
      common_vo.message = "Invalid email format";
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

  void handle_register_request(const utils::Context& ctx, const UserRegisterDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 校验参数
    if (dto.nickname.empty() || dto.email.empty() || dto.password.empty() || dto.confirm_password.empty() ||
        dto.verify_code.empty() || dto.purpose != 1)
    {
      logger.error("{}: Invalid parameters", request_id);
      common_vo.code = utils::INVALID_PARAMETERS;
      common_vo.message = "Invalid parameters";
      common_vo.data = "";
      return;
    }

    // 验证邮箱格式
    if (!is_valid_email(dto.email))
    {
      logger.error("{}: Invalid email format", request_id);
      common_vo.code = utils::INVALID_EMAIL_FORMAT;
      common_vo.message = "Invalid email format";
      common_vo.data = "";
      return;
    }

    // 两次密码需要相同
    if (dto.password != dto.confirm_password)
    {
      logger.error("{}: Passwords do not equal to confirm password", request_id);
      common_vo.code = utils::PASSWORDS_UNEQUAL_CONFIRM;
      common_vo.message = "Passwords do not equal to confirm password";
      common_vo.data = "";
      return;
    }

    // 密码是否有效
    if (!is_valid_password(dto.password))
    {
      logger.error("{}: Invalid password format", request_id);
      common_vo.code = utils::INVALID_PASSWORD_FORMAT;
      common_vo.message = "Invalid password format, must be at least 6 characters with letters and numbers";
      common_vo.data = "";
      return;
    }

    // 调用 Service 处理
    user_service.HandleRegisterRequest(ctx, dto, common_vo);
    if (common_vo.code != utils::SUCCESS)
    {
      return;
    }

    common_vo.code = utils::SUCCESS;
    common_vo.message = "User registered successfully";
    common_vo.data = "";
  }

  void handle_reset_pass_request(const utils::Context& ctx, const UserResetPasswordDTO& dto,
                                 core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

    // 校验参数
    if (dto.email.empty() || dto.password.empty() || dto.confirm_password.empty() || dto.verify_code.empty() ||
        dto.purpose != 2)
    {
      logger.error("{}: Invalid parameters", request_id);
      common_vo.code = utils::INVALID_PARAMETERS;
      common_vo.message = "Invalid parameters";
      common_vo.data = "";
      return;
    }

    // 验证邮箱格式
    if (!is_valid_email(dto.email))
    {
      logger.error("{}: Invalid email format", request_id);
      common_vo.code = utils::INVALID_EMAIL_FORMAT;
      common_vo.message = "Invalid email format";
      common_vo.data = "";
      return;
    }

    // 两次密码需要相同
    if (dto.password != dto.confirm_password)
    {
      logger.error("{}: Passwords do not equal to confirm password", request_id);
      common_vo.code = utils::PASSWORDS_UNEQUAL_CONFIRM;
      common_vo.message = "Passwords do not equal to confirm password";
      common_vo.data = "";
      return;
    }

    // 密码是否有效
    if (!is_valid_password(dto.password))
    {
      logger.error("{}: Invalid password format", request_id);
      common_vo.code = utils::INVALID_PASSWORD_FORMAT;
      common_vo.message = "Invalid password format, must be at least 6 characters with letters and numbers";
      common_vo.data = "";
      return;
    }

    // 调用 Service 处理
    user_service.HandleResetRequest(ctx, dto, common_vo);
    if (common_vo.code != utils::SUCCESS)
    {
      return;
    }

    common_vo.code = utils::SUCCESS;
    common_vo.message = "Password reset successfully";
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

void UserController::HandleRegisterRequest(const utils::Context& ctx, const UserRegisterDTO& dto,
                                           core::CommonVO& common_vo) const
{
  _pimpl->handle_register_request(ctx, dto, common_vo);
}

void UserController::HandleResetPassRequest(const utils::Context& ctx, const UserResetPasswordDTO& dto,
                                            core::CommonVO& common_vo) const
{
  _pimpl->handle_reset_pass_request(ctx, dto, common_vo);
}

}  // namespace core