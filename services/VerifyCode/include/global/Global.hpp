/******************************************************************************
 *
 * @file       Global.hpp
 * @brief      这里写全局的声明配置，宏定义集中处，以及各个文件的魔法数字
 *
 * @author     KBchulan
 * @date       2025/04/03
 * @history
 ******************************************************************************/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace global
{

// ================
// 雪花算法相关常量
// ================
namespace snowflake
{
constexpr std::uint64_t EPOCH = 1704067200000ULL;         // 起始时间戳 2024-01-01 00:00:00 UTC
constexpr std::uint64_t MAX_CLOCK_ROLLBACK_MS = 5000ULL;  // 最大允许时钟回拨 5秒
constexpr std::uint64_t MAX_ROLLBACK_WAIT_MS = 100ULL;    // 最大回拨等待时间 100ms
constexpr std::chrono::microseconds SLEEP_INTERVAL{100};  // 等待间隔
}  // namespace snowflake

// ================
// 日志系统相关常量
// ================
namespace logger
{
constexpr std::size_t MAX_MESSAGE_SIZE = 512;             // 单条日志消息最大长度
constexpr std::size_t QUEUE_CAPACITY = 16384;             // 日志队列容量 2^14
constexpr std::chrono::microseconds FLUSH_INTERVAL{100};  // 刷新等待间隔
}  // namespace logger

// ================
// 网络服务相关常量
// ================
namespace server
{
constexpr const char* SERVER_ADDRESS = "0.0.0.0:10002";               // gRPC 服务器监听地址
constexpr const char* SMTP_QQ_EMAIL_URL = "smtps://smtp.qq.com:465";  // QQ邮箱 SMTP 服务器地址
constexpr const char* SMTP_QQ_EMAIL_USER = "2262317520@qq.com";       // QQ邮箱账号
constexpr const char* SMTP_QQ_EMAIL_CODE = "cwkuekbiwcvlebdi";        // QQ邮箱 SMTP 授权码

constexpr const char* REDIS_SERVER_HOST = "127.0.0.1";  // Redis 服务器地址
constexpr std::uint16_t REDIS_SERVER_PORT = 6379;       // Redis 服务器端口
constexpr const char* REDIS_SERVER_PASSWORD = "whx";    // Redis 服务器密码

constexpr const char* VERIFY_CODE_REDIS_KEY_PREFIX = "verify_code_";  // 验证码在 Redis 中的键前缀
constexpr int VERIFY_CODE_TTL = 60;                                   // 验证码在 Redis 中的过期时间 60 秒
}  // namespace server

}  // namespace global

#endif  // GLOBAL_HPP
