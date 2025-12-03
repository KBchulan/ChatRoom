/******************************************************************************
 *
 * @file       user_controller.hpp
 * @brief      user 模块的控制器
 *
 * @author     KBchulan
 * @date       2025/12/03
 * @history
 ******************************************************************************/

#ifndef USER_CONTROLLER_HPP
#define USER_CONTROLLER_HPP

#include <core/CoreExport.hpp>
#include <core/domain/dto/user/user_dto.hpp>
#include <core/domain/vo/common_vo.hpp>
#include <memory>

namespace core
{

class CORE_EXPORT UserController
{
public:
  UserController();
  ~UserController();

  static UserController& GetInstance();

  void HandleSendCodeRequest(const UserSendCodeDTO& dto, core::CommonVO& common_vo) const;

  UserController(const UserController&) = delete;
  UserController& operator=(const UserController&) = delete;
  UserController(UserController&&) = delete;
  UserController& operator=(UserController&&) = delete;

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // USER_CONTROLLER_HPP