#include "user_controller.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <regex>
#pragma GCC diagnostic pop

#include <utils/common/code.hpp>

namespace core
{

struct UserController::_impl
{
  [[nodiscard]] static bool is_valid_email(const std::string& email)
  {
    // 基本的邮箱正则表达式
    static const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
  }

  void handle_send_code_request(const UserSendCodeDTO& dto, core::CommonVO& common_vo) const
  {
    void(this);

    // 校验参数
    if (dto.email.empty() || (dto.purpose != 1 && dto.purpose != 2 && dto.purpose != 3))
    {
      common_vo.code = utils::INVALID_PARAMETERS;
      common_vo.message = "Invalid parameters";
      common_vo.data = "";
      return;
    }

    if (!is_valid_email(dto.email))
    {
      common_vo.code = utils::INVALID_EMAIL_FORMAT;
      common_vo.message = "Invalid email format";
      common_vo.data = "";
      return;
    }

    // TODO: 调用 service 层发送验证码

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

void UserController::HandleSendCodeRequest(const UserSendCodeDTO& dto, core::CommonVO& common_vo) const
{
  _pimpl->handle_send_code_request(dto, common_vo);
}

}  // namespace core