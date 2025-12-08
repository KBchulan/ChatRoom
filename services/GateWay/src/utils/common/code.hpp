/******************************************************************************
 *
 * @file       code.hpp
 * @brief      返回状态码集中定义
 *
 * @author     KBchulan
 * @date       2025/12/02
 * @history
 ******************************************************************************/

#ifndef CODE_HPP
#define CODE_HPP

#include <cstdint>

namespace utils
{

constexpr std::int16_t SUCCESS = 0;                 // 成功
constexpr std::int16_t URL_NOT_FOUND = 1;           // URL未找到
constexpr std::int16_t UUID_GENERATION_FAILED = 2;  // UUID生成失败
constexpr std::int16_t INVALID_PARAMETERS = 3;      // 参数无效
constexpr std::int16_t JSON_PARSE_ERROR = 4;        // JSON解析错误
constexpr std::int16_t INVALID_EMAIL_FORMAT = 5;    // 邮箱格式无效
constexpr std::int16_t SEND_EMAIL_CODE_FAILED = 6;  // 发送验证码失败
constexpr std::int16_t DATABASE_ERROR = 7;          // 数据库错误
constexpr std::int16_t REDIS_ERROR = 8;             // Redis错误
constexpr std::int16_t CODE_ALREADY_SENT = 9;       // 验证码已发送

}  // namespace utils

#endif  // CODE_HPP