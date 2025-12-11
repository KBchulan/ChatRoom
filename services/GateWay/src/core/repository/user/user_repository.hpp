/******************************************************************************
 *
 * @file       user_repository.hpp
 * @brief      用户模块的数据层，负责落盘行为
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef USER_REPOSITORY_HPP
#define USER_REPOSITORY_HPP

#include <core/CoreExport.hpp>
#include <core/domain/do/user/user_do.hpp>
#include <expected>
#include <memory>

namespace core
{

class CORE_EXPORT UserRepository
{
public:
  static UserRepository& GetInstance();

  [[nodiscard]] bool InsertVerifyCode(const UserVerifyCodeDO& user_verify_code_do) const;

  [[nodiscard]] std::expected<bool, std::string> CheckUserExists(const std::string& email,
                                                                 const std::string& nickname) const;

  [[nodiscard]] bool InsertUser(const UserDO& user_do) const;

  [[nodiscard]] bool UpdateUserPassword(const std::string& email, const std::string& password_hash) const;

  UserRepository(const UserRepository&) = delete;
  UserRepository& operator=(const UserRepository&) = delete;
  UserRepository(UserRepository&&) = delete;
  UserRepository& operator=(UserRepository&&) = delete;

private:
  UserRepository();
  ~UserRepository();

  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // USER_REPOSITORY_HPP