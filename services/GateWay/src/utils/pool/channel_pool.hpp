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
  ChannelPool(const std::string& target, std::size_t pool_size)
  {
    _channels.reserve(pool_size);

    for (std::size_t i = 0; i < pool_size; ++i)
    {
      // 设置发送和接收消息的最大大小都为 4MB
      grpc::ChannelArguments args;
      args.SetMaxReceiveMessageSize(global::server::RPC_MAX_SEND_RECV_SIZE);
      args.SetMaxSendMessageSize(global::server::RPC_MAX_SEND_RECV_SIZE);

      _channels.emplace_back(grpc::CreateCustomChannel(target, grpc::InsecureChannelCredentials(), args));
    }
  }

  ~ChannelPool() = default;

  [[nodiscard]] std::shared_ptr<grpc::Channel> GetChannel()
  {
    std::size_t index = _counter.fetch_add(1, std::memory_order_relaxed) % _channels.size();
    return _channels[index];
  }

private:
  std::vector<std::shared_ptr<grpc::Channel>> _channels;
  std::atomic<std::size_t> _counter{0};
};

}  // namespace utils

#endif  // CHANNEL_POOL_HPP