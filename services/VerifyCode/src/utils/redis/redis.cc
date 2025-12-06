#include "redis.hpp"

#include <hiredis/hiredis.h>

namespace utils
{

RedisClient::RedisClient(std::string_view host, int port, std::string_view password)
    : _ctx(redisConnect(std::string(host).c_str(), port))
{
  if (_ctx != nullptr && !password.empty())
  {
    auto* redis_ctx = static_cast<redisContext*>(_ctx);
    if (redis_ctx->err == 0)
    {
      auto* reply = static_cast<redisReply*>(redisCommand(redis_ctx, "AUTH %b", password.data(), password.size()));
      if (reply != nullptr)
      {
        freeReplyObject(reply);
      }
    }
  }
}

RedisClient::~RedisClient()
{
  if (_ctx != nullptr)
  {
    redisFree(static_cast<redisContext*>(_ctx));
  }
}

std::string RedisClient::Set(std::string_view key, std::string_view value, int expire_seconds) const
{
  if (!IsConnected())
  {
    return "redis not connected";
  }

  auto* redis_ctx = static_cast<redisContext*>(_ctx);
  auto* reply = static_cast<redisReply*>(
      redisCommand(redis_ctx, "SET %b %b EX %d", key.data(), key.size(), value.data(), value.size(), expire_seconds));

  if (reply == nullptr)
  {
    return "redis command failed";
  }

  bool success = (reply->type == REDIS_REPLY_STATUS && std::string_view(reply->str) == "OK");
  freeReplyObject(reply);

  if (!success)
  {
    return "redis SET failed";
  }

  return "";
}

auto RedisClient::IsConnected() const -> bool
{
  if (_ctx == nullptr)
  {
    return false;
  }
  const auto* redis_ctx = static_cast<redisContext*>(_ctx);
  return redis_ctx->err == 0;
}

}  // namespace utils
