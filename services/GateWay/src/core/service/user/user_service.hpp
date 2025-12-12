/******************************************************************************
 *
 * @file       user_service.hpp
 * @brief      user 服务的 service 层
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef USER_SERVICE_HPP
#define USER_SERVICE_HPP

#include <core/CoreExport.hpp>
#include <core/domain/dto/user/user_dto.hpp>
#include <core/domain/vo/common_vo.hpp>
#include <core/domain/vo/user/user_vo.hpp>
#include <utils/context/context.hpp>

namespace core
{

class CORE_EXPORT UserService
{
public:
  static UserService& GetInstance();

  void HandleSendCodeRequest(const utils::Context& ctx, const UserSendCodeDTO& dto, core::CommonVO& common_vo) const;
  void HandleRegisterRequest(const utils::Context& ctx, const UserRegisterDTO& dto, core::CommonVO& common_vo) const;
  void HandleResetRequest(const utils::Context& ctx, const UserResetPasswordDTO& dto, core::CommonVO& common_vo) const;
  void HandleLoginRequest(const utils::Context& ctx, const UserLoginDTO& dto, core::CommonVO& common_vo,
                          UserLoginVO& login_vo) const;

  UserService(const UserService&) = delete;
  UserService& operator=(const UserService&) = delete;
  UserService(UserService&&) = delete;
  UserService& operator=(UserService&&) = delete;

private:
  UserService();
  ~UserService();

  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // USER_SERVICE_HPP