#include "server.hpp"

#include <algorithm>
#include <tools/Id.hpp>
#include <tools/Logger.hpp>

namespace core
{

namespace
{

std::string generate_token()
{
  auto token = tools::UuidGenerator::generateUuid();
  if (!token)
  {
    tools::Logger::getInstance().error("Failed to generate token");
    return "";
  }
  return token.value();
}

};  // namespace

grpc::Status StatusServiceImpl::GetTcpServer([[maybe_unused]] grpc::ServerContext* ctx,
                                             const GetTcpServerRequest* request, GetTcpServerResponse* response)
{
  const auto& uuid = request->uuid();

  // 校验参数
  if (uuid.empty())
  {
    return {grpc::StatusCode::INVALID_ARGUMENT, "Failed to get uuid from grpc client"};
  }

  // 先判断是否请求过，直接返回原来分配的，防止首次连接失败
  {
    std::lock_guard lock(_mutex);
    if (_pending_connections.contains(uuid))
    {
      const auto& info = _pending_connections[uuid];
      const auto& server = _tcp_servers[info.server_index];
      response->set_code(0);
      response->set_message("Get TCP server success");
      response->mutable_data()->insert({"host", server.host});
      response->mutable_data()->insert({"port", std::to_string(server.port)});
      response->mutable_data()->insert({"token", info.token});
      return grpc::Status::OK;
    }
  }

  // 生成 token
  const auto token = generate_token();
  if (token.empty())
  {
    return {grpc::StatusCode::INTERNAL, "Failed to generate token"};
  }

  std::string host;
  std::string port;
  {
    std::lock_guard lock(_mutex);

    // 为其选择一个合适的 tcp server，先返回给它信息，随后再真正连接时再增加计数
    auto iter = std::ranges::min_element(_tcp_servers, {}, &TcpServerInfo::connection_count);
    auto server_index = static_cast<size_t>(std::distance(_tcp_servers.begin(), iter));
    const auto& server = *iter;

    // 存储 uuid -> {token, server_index} 的映射
    _pending_connections[uuid] = {.token = token, .server_index = server_index};

    host = server.host;
    port = std::to_string(server.port);
  }

  response->set_code(0);
  response->set_message("Get TCP server success");
  response->mutable_data()->insert({"host", host});
  response->mutable_data()->insert({"port", port});
  response->mutable_data()->insert({"token", token});

  tools::Logger::getInstance().info("GetTcpServer: uuid = {}, token = {}, address = {}:{}", uuid, token, host, port);
  return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::LoginVerify([[maybe_unused]] grpc::ServerContext* ctx,
                                            const LoginVerifyRequest* request, LoginVerifyResponse* response)
{
  const auto& uuid = request->uuid();
  const auto& token = request->token();

  {
    std::lock_guard lock(_mutex);

    // 验证 token
    if (!_pending_connections.contains(uuid) || _pending_connections[uuid].token != token)
    {
      tools::Logger::getInstance().error("LoginVerify error: uuid = {}, token = {}", uuid, token);
      return {grpc::StatusCode::PERMISSION_DENIED, "Invalid token"};
    }

    // 验证通过后，增加该 tcp server 的连接数并移除 token
    auto server_index = _pending_connections[uuid].server_index;
    _tcp_servers[server_index].connection_count++;
    _pending_connections.erase(uuid);
  }

  response->set_code(0);
  response->set_message("Login verify success");
  return grpc::Status::OK;
}

}  // namespace core