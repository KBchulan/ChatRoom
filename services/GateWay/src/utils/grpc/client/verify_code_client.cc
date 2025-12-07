#include "verify_code_client.hpp"

namespace utils
{

VerifyCodeClient& VerifyCodeClient::GetInstance()
{
  static VerifyCodeClient instance;
  return instance;
}

void VerifyCodeClient::Init(const std::string& server_address, std::size_t pool_size)
{
  _pool = std::make_unique<ChannelPool>(server_address, pool_size);
}

VerifyCodeResult VerifyCodeClient::SendVerifyCode(std::string_view email)
{
  auto stub = create_stub();

  VerifyCodeRequest request;
  request.set_email(std::string(email));

  VerifyCodeResponse response;
  grpc::ClientContext context;

  auto status = stub->VerifyCode(&context, request, &response);

  if (status.ok())
  {
    return response;
  }
  return std::unexpected(GrpcError{.code = status.error_code(), .message = status.error_message()});
}

std::unique_ptr<VerifyCodeService::Stub> VerifyCodeClient::create_stub()
{
  return VerifyCodeService::NewStub(_pool->GetChannel());
}

}  // namespace utils