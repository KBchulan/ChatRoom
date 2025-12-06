#include <grpcpp/grpcpp.h>

#include <core/server/server.hpp>
#include <core/smtp/smtp.hpp>
#include <global/Global.hpp>
#include <tools/Logger.hpp>

int main()
{
  try
  {
    // 创建builder，配置选项
    grpc::ServerBuilder builder;
    builder.AddListeningPort(global::server::SERVER_ADDRESS, grpc::InsecureServerCredentials());

    std::string qq_email_url = global::server::SMTP_QQ_EMAIL_URL;
    std::string qq_email_user = global::server::SMTP_QQ_EMAIL_USER;
    std::string qq_email_code = global::server::SMTP_QQ_EMAIL_CODE;
    auto smtp_client = std::make_unique<core::SmtpClient>(qq_email_url, qq_email_user, qq_email_code);

    // 注册服务
    core::VerifyCodeServiceImpl service(std::move(smtp_client));
    builder.RegisterService(&service);

    // 创建一个服务
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    tools::Logger::getInstance().info("VerifyCode Server listening on {}", global::server::SERVER_ADDRESS);
    server->Wait();
  }
  catch (const std::exception& e)
  {
    tools::Logger::getInstance().error("Unhandled exception in main: {}", e.what());
    return EXIT_FAILURE;
  }
}