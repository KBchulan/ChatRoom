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

LoginVerifyResult StatusServerClinet::VerifyLoginInfo(const std::string& uuid, const std::string& token)
{
  auto stub = create_stub();

  LoginVerifyRequest request;
  request.set_uuid(uuid);
  request.set_token(token);

  LoginVerifyResponse response;
  grpc::ClientContext context;

  auto status = stub->LoginVerify(&context, request, &response);
  if (status.ok())
  {
    return response;
  }

  return std::unexpected{GrpcError{.code = status.error_code(), .message = status.error_message()}};
}

std::unique_ptr<StatusService::Stub> StatusServerClinet::create_stub()
{
  return StatusService::NewStub(_pool->GetChannel());
}

}  // namespace utils