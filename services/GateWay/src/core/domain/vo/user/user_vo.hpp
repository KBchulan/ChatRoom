/******************************************************************************
 *
 * @file       user_vo.hpp
 * @brief      user 模块的请求响应
 *
 * @author     KBchulan
 * @date       2025/12/03
 * @history
 ******************************************************************************/

#ifndef USER_VO_HPP
#define USER_VO_HPP

#include <json/value.h>

#include <core/CoreExport.hpp>
#include <cstdint>
#include <string>

namespace core
{

struct CORE_EXPORT UserLoginVO
{
  std::string uuid;
  std::string host;
  std::int16_t port;
  std::string token;
  std::string jwt_token;

  [[nodiscard]] Json::Value ToJson() const
  {
    Json::Value root;
    root["uuid"] = uuid;
    root["host"] = host;
    root["port"] = port;
    root["token"] = token;
    root["jwt_token"] = jwt_token;
    return root;
  }
};

}  // namespace core

#endif  // USER_VO_HPP