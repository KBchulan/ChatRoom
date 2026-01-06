/******************************************************************************
 *
 * @file       user_do.hpp
 * @brief      用户模块的基本数据结构
 *
 * @author     KBchulan
 * @date       2025/12/16
 * @history
 ******************************************************************************/

#ifndef USER_DO_HPP
#define USER_DO_HPP

#include <core/CoreExport.hpp>
#include <cstdint>
#include <string>

namespace core
{

struct CORE_EXPORT UserDO
{
  std::int64_t id;
  std::string uuid;
  std::string nickname;
  std::string avatar;
  std::string email;
  std::string password_hash;
  std::string last_login;
  std::string created_at;
  std::string updated_at;
};

}  // namespace core

#endif  // USER_DO_HPP