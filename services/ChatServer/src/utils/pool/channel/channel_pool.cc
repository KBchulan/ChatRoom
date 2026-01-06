#include "channel_pool.hpp"

#include <global/Global.hpp>
#include <tools/Logger.hpp>

namespace utils
{

ChannelPool::ChannelPool(const std::string& target, std::size_t pool_size)
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

  tools::Logger::getInstance().info("channel pool init successful");
}

ChannelPool::~ChannelPool()
{
  tools::Logger::getInstance().info("channel pool closed");
}

std::shared_ptr<grpc::Channel> ChannelPool::GetChannel()
{
  std::size_t index = _counter.fetch_add(1, std::memory_order_relaxed) % _channels.size();
  return _channels[index];
}

}  // namespace utils
