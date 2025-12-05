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

}  // namespace core

#endif  // USER_DTO_HPP