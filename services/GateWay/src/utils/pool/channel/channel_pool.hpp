/******************************************************************************
 *
 * @file       channel_pool.hpp
 * @brief      GRPC 连接池子
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef CHANNEL_POOL_HPP
#define CHANNEL_POOL_HPP

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>

#include <atomic>
#include <cstddef>
#include <global/Global.hpp>
#include <memory>
#include <utils/UtilsExport.hpp>
#include <vector>

namespace utils
{

class UTILS_EXPORT ChannelPool
{
public:
  ChannelPool(const std::string& target, std::size_t pool_size);

  ~ChannelPool();

  [[nodiscard]] std::shared_ptr<grpc::Channel> GetChannel();

private:
  std::vector<std::shared_ptr<grpc::Channel>> _channels;
  std::atomic<std::size_t> _counter{0};
};

}  // namespace utils

#endif  // CHANNEL_POOL_HPP