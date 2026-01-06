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
constexpr bool ENABLE_FILE_LOG = true;                    // 是否启用文件日志
constexpr std::chrono::microseconds FLUSH_INTERVAL{100};  // 刷新等待间隔
}  // namespace logger

// ================
// 服务器相关常量
// ================
namespace server
{

constexpr unsigned short DEFAULT_SERVER_PORT = 10004;  // 默认服务器端口
constexpr std::int8_t IO_CONTEXT_POOL_SIZE = 8;        // io_context 池子大小
constexpr std::int8_t MSG_TYPE_LENGTH = 2;             // 消息类型长度
constexpr std::int8_t MSG_LEN_LENGTH = 2;              // 消息长度字段长度
constexpr std::int8_t MSG_HEAD_TOTAL_LEN = 4;          // 消息头总长度
constexpr std::size_t RECV_BUFFER_SIZE = 8192;         // 接收缓冲区大小
constexpr std::size_t SEND_QUEUE_CAPACITY = 1024;      // 发送队列容量 2^10
constexpr std::size_t SEND_EXPIRE_TIME_S = 60 * 5;     // 单条发送过期时间 300s
constexpr std::size_t LOGIC_QUEUE_CAPACITY = 4096;     // 逻辑队列容量 2^12

constexpr std::int32_t RPC_MAX_SEND_RECV_SIZE = 4 * 1024 * 1024;  // RPC 最大发送和接收消息大小 4MB

constexpr const char* STATUS_RPC_SERVER_HOST = "127.0.0.1";  // 状态 RPC 服务器地址
constexpr std::uint16_t STATUS_RPC_SERVER_PORT = 10003;      // 状态 RPC 服务器端口
constexpr std::size_t STATUS_RPC_CONNECTION_POOL_SIZE = 8;   // 状态 RPC 连接池大小

constexpr const char* DB_HOST = "127.0.0.1";  // 数据库主机地址
constexpr std::uint16_t DB_PORT = 3306;       // 数据库端口
constexpr const char* DB_USER = "root";       // 数据库用户名
constexpr const char* DB_PASSWORD = "whx";    // 数据库密码
constexpr const char* DB_NAME = "chatroom";   // 数据库名称
constexpr std::size_t DB_MAX_POOL_SIZE = 16;  // 数据库最大连接池大小

constexpr const char* REDIS_HOST = "127.0.0.1";  // Redis 主机地址
constexpr std::uint16_t REDIS_PORT = 6379;       // Redis 端口
constexpr const char* REDIS_PASSWORD = "whx";    // Redis 密码
constexpr std::size_t REDIS_DB_INDEX = 0;        // Redis 数据库索引
constexpr std::size_t REDIS_MAX_POOL_SIZE = 16;  // Redis 最大连接池大小
constexpr std::size_t REDIS_TIMEOUT = 3;         // Redis 连接超时时间

constexpr const char* USER_INFO_PREFIX = "user_info:";  // 用户信息前缀
constexpr std::size_t USER_INFO_EXPIRE_TIME_S = 3600;   // 用户信息过期时间 1小时

}  // namespace server

}  // namespace global

#endif  // GLOBAL_HPP
