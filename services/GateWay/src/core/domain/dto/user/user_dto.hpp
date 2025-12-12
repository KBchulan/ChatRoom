/******************************************************************************
 *
 * @file       user_dto.hpp
 * @brief      用户模块的 DTO 负载
 *
 * @author     KBchulan
 * @date       2025/12/03
 * @history
 ******************************************************************************/

#ifndef USER_DTO_HPP
#define USER_DTO_HPP

#include <core/CoreExport.hpp>
#include <cstdint>
#include <utils/common/func.hpp>

namespace core
{

struct CORE_EXPORT UserSendCodeDTO
{
  std::string email;
  std::int8_t purpose;

  static std::optional<UserSendCodeDTO> FromJsonString(const std::string& json_str)
  {
    auto root = utils::ParseJson(json_str);
    if (!root || !(*root).isObject())
    {
      return std::nullopt;
    }

    return UserSendCodeDTO{.email = (*root)["email"].asString(),
                           .purpose = static_cast<std::int8_t>((*root)["purpose"].asInt())};
  }
};

struct CORE_EXPORT UserRegisterDTO
{
  std::string nickname;
  std::string email;
  std::string password;
  std::string confirm_password;
  std::string verify_code;
  std::int8_t purpose;

  static std::optional<UserRegisterDTO> FromJsonString(const std::string& json_str)
  {
    auto root = utils::ParseJson(json_str);
    if (!root || !(*root).isObject())
    {
      return std::nullopt;
    }

    return UserRegisterDTO{.nickname = (*root)["nickname"].asString(),
                           .email = (*root)["email"].asString(),
                           .password = (*root)["password"].asString(),
                           .confirm_password = (*root)["confirm_password"].asString(),
                           .verify_code = (*root)["verify_code"].asString(),
                           .purpose = static_cast<std::int8_t>((*root)["purpose"].asInt())};
  }
};

struct CORE_EXPORT UserResetPasswordDTO
{
  std::string email;
  std::string password;
  std::string confirm_password;
  std::string verify_code;
  std::int8_t purpose;

  static std::optional<UserResetPasswordDTO> FromJsonString(const std::string& json_str)
  {
    auto root = utils::ParseJson(json_str);
    if (!root || !(*root).isObject())
    {
      return std::nullopt;
    }

    return UserResetPasswordDTO{.email = (*root)["email"].asString(),
                                .password = (*root)["password"].asString(),
                                .confirm_password = (*root)["confirm_password"].asString(),
                                .verify_code = (*root)["verify_code"].asString(),
                                .purpose = static_cast<std::int8_t>((*root)["purpose"].asInt())};
  }
};

struct CORE_EXPORT UserLoginDTO
{
  std::string user;
  std::string password;
  bool is_email = false;

  static std::optional<UserLoginDTO> FromJsonString(const std::string& json_str)
  {
    auto root = utils::ParseJson(json_str);
    if (!root || !(*root).isObject())
    {
      return std::nullopt;
    }

    return UserLoginDTO{.user = (*root)["user"].asString(), .password = (*root)["password"].asString()};
  }
};

}  // namespace core

#endif  // USER_DTO_HPP