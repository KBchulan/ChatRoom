#include "status_server_client.hpp"

namespace utils
{

StatusServerClinet& StatusServerClinet::GetInstance()
{
  static StatusServerClinet instance;
  return instance;
}

void StatusServerClinet::Init(const std::string& server_address, std::size_t pool_size)
{
  _pool = std::make_unique<ChannelPool>(server_address, pool_size);
}

GetTcpServerResult StatusServerClinet::GetTcpServer(std::string_view uuid)
{
  auto stub = create_stub();

  GetTcpServerRequest request;
  request.set_uuid(std::string(uuid));

  GetTcpServerResponse response;
  grpc::ClientContext context;

  auto status = stub->GetTcpServer(&context, request, &response);
  if (status.ok())
  {
    return response;
  }
  return std::unexpected(GrpcError{.code = status.error_code(), .message = status.error_message()});
}

std::unique_ptr<StatusService::Stub> StatusServerClinet::create_stub()
{
  return StatusService::NewStub(_pool->GetChannel());
}

}  // namespace utils