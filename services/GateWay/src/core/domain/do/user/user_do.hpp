/******************************************************************************
 *
 * @file       user_do.hpp
 * @brief      用户模块的 do 结构
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef USER_DO_HPP
#define USER_DO_HPP

#include <chrono>
#include <cstdint>
#include <string>

namespace core
{

struct UserVerifyCodeDO
{
  std::int64_t id;
  std::string email;
  std::string code;
  std::int8_t purpose;  // 1: 注册, 2: 重置密码, 3: 登录
  std::chrono::system_clock::time_point created_at;
};

}  // namespace core

#endif  // USER_DO_HPP