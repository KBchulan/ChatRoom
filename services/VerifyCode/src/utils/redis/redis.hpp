/******************************************************************************
 *
 * @file       redis.hpp
 * @brief      Redis client wrapper
 *
 * @author     KBchulan
 * @date       2025/12/05
 * @history
 ******************************************************************************/

#ifndef REDIS_HPP
#define REDIS_HPP

#include <string>
#include <string_view>
#include <utils/UtilsExport.hpp>

namespace utils
{

class UTILS_EXPORT RedisClient
{
public:
  RedisClient(std::string_view host, int port, std::string_view password = "");
  ~RedisClient();

  RedisClient(const RedisClient&) = delete;
  RedisClient& operator=(const RedisClient&) = delete;
  RedisClient(RedisClient&&) noexcept = delete;
  RedisClient& operator=(RedisClient&&) noexcept = delete;

  // SET key value EX expire_seconds
  [[nodiscard]] std::string Set(std::string_view key, std::string_view value, int expire_seconds) const;

  // check connection
  [[nodiscard]] auto IsConnected() const -> bool;

private:
  void* _ctx;  // redisContext*
};

}  // namespace utils

#endif  // REDIS_HPP
