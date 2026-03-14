/******************************************************************************
 *
 * @file       user_manager.hpp
 * @brief      用户管理，用于后续踢人等逻辑的实现
 *
 * @author     KBchulan
 * @date       2026/03/02
 * @history
 ******************************************************************************/

#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <core/CoreExport.hpp>
#include <memory>

namespace core
{

class CORE_EXPORT UserManager
{
public:
  UserManager();
  ~UserManager();

  static UserManager& GetInstance();

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // USER_MANAGER_HPP