#include "chat_server_client.hpp"

namespace utils
{

ChatServerClient& ChatServerClient::GetInstance()
{
  static ChatServerClient instance;
  return instance;
}

void ChatServerClient::Init(const std::string& server_address, std::size_t pool_size)
{
  _pool = std::make_unique<ChannelPool>(server_address, pool_size);
}

std::unique_ptr<ChatService::Stub> ChatServerClient::create_stub()
{
  return ChatService::NewStub(_pool->GetChannel());
}

}  // namespace utils