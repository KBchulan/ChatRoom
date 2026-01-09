/******************************************************************************
 *
 * @file       user_repository.hpp
 * @brief      用户模块的查询
 *
 * @author     KBchulan
 * @date       2025/12/16
 * @history
 ******************************************************************************/

#ifndef USER_REPOSITORY_HPP
#define USER_REPOSITORY_HPP

#include <core/CoreExport.hpp>
#include <core/model/user_do.hpp>

namespace core
{

class CORE_EXPORT UserRepository
{
public:
  static UserDO getUserById(const std::string& userId);
  static bool updateLastLogin(const std::string& userId);
};

}  // namespace core

#endif  // USER_REPOSITORY_HPP