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

  // 存储该 uuid -> token 的映射关系
  auto token = generate_token();
  if (token.empty())
  {
    return {grpc::StatusCode::INTERNAL, "Failed to generate token"};
  }
  _tokens[uuid] = token;

  // 为其选择一个合适的 tcp server，先返回给它信息，随后再真正连接时再增加计数
  auto& server = *std::ranges::min_element(_tcp_servers, {}, &TcpServerInfo::connection_count);

  response->mutable_data()->insert({"host", server.host});
  response->mutable_data()->insert({"port", std::to_string(server.port)});
  response->mutable_data()->insert({"token", token});

  tools::Logger::getInstance().info("GetTcpServer: uuid = {}, token = {}", uuid, token);
  return grpc::Status::OK;
}

}  // namespace core