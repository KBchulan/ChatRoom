/******************************************************************************
 *
 * @file       code.hpp
 * @brief      一些常量码的定义
 *
 * @author     KBchulan
 * @date       2025/12/15
 * @history
 ******************************************************************************/

#ifndef CODE_HPP
#define CODE_HPP

#include <cstdint>

namespace utils
{

// 错误码定义
constexpr std::int16_t SUCCESS = 0;           // 成功
constexpr std::int16_t JSON_PARSE_ERROR = 1;  // JSON 解析错误
constexpr std::int16_t REDIS_ERROR = 2;       // Redis 错误

// 消息ID定义
constexpr std::int16_t ID_LOGIN_CHAT = 1005;           // 逻辑登录
constexpr std::int16_t ID_LOGIN_CHAT_RESPONSE = 1006;  // 逻辑登录回包
constexpr std::int16_t ID_EXIT_LOGIN = 1007;           // 退出登录
constexpr std::int16_t ID_EXIT_LOGIN_RESPONSE = 1008;  // 退出登录回包

}  // namespace utils

#endif  // CODE_HPP