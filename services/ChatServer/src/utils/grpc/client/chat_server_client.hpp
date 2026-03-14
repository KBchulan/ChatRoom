/******************************************************************************
 *
 * @file       chat_server_client.hpp
 * @brief      聊天服务器之间进行通信的客户端
 *
 * @author     KBchulan
 * @date       2026/03/02
 * @history
 ******************************************************************************/

#ifndef CHAT_SERVER_CLIENT_HPP
#define CHAT_SERVER_CLIENT_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/grpc/chat_server/chat_server.grpc.pb.h>
#include <utils/grpc/chat_server/chat_server.pb.h>
#pragma GCC diagnostic pop

#include <memory>
#include <utils/UtilsExport.hpp>
#include <utils/pool/channel/channel_pool.hpp>

namespace utils
{

using namespace KBchulan::ChatRoom::ChatServer;

class UTILS_EXPORT ChatServerClient
{
public:
  static ChatServerClient& GetInstance();
  void Init(const std::string& server_address, std::size_t pool_size);

  ChatServerClient(const ChatServerClient&) = delete;
  ChatServerClient& operator=(const ChatServerClient&) = delete;
  ChatServerClient(ChatServerClient&&) = delete;
  ChatServerClient& operator=(ChatServerClient&&) = delete;

private:
  ChatServerClient() = default;
  ~ChatServerClient() = default;

  [[nodiscard]] std::unique_ptr<ChatService::Stub> create_stub();

  std::unique_ptr<ChannelPool> _pool;
};

}  // namespace utils

#endif  // CHAT_SERVER_CLIENT_HPP