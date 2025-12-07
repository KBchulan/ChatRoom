#include "user_service.hpp"

#include <core/domain/do/user/user_do.hpp>
#include <core/repository/user/user_repository.hpp>
#include <tools/Logger.hpp>
#include <utils/common/code.hpp>
#include <utils/grpc/client/verify_code_client.hpp>

namespace core
{

struct UserService::_impl
{
  tools::Logger& logger = tools::Logger::getInstance();
  UserRepository& _user_repository = UserRepository::GetInstance();
  utils::VerifyCodeClient& _verify_code_client = utils::VerifyCodeClient::GetInstance();

  void handle_send_code_request(const utils::Context& ctx, const UserSendCodeDTO& dto, core::CommonVO& common_vo) const
  {
    auto request_id = std::any_cast<std::string>(ctx.Get("request_id"));

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

}  // namespace core