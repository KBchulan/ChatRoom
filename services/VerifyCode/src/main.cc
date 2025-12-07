#include <grpcpp/grpcpp.h>

#include <atomic>
#include <condition_variable>
#include <core/server/server.hpp>
#include <core/smtp/smtp.hpp>
#include <csignal>
#include <global/Global.hpp>
#include <mutex>
#include <thread>
#include <tools/Logger.hpp>

namespace
{

std::atomic<int> g_received_signal{0};
std::mutex g_mutex;
std::condition_variable g_cv;

void signal_handler(int signal)
{
  g_received_signal.store(signal);
  g_cv.notify_one();
}

}  // namespace

int main()
{
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

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

    // 创建服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    tools::Logger::getInstance().info("VerifyCode Server listening on {}", global::server::SERVER_ADDRESS);

    std::thread shutdown_thread(
        [&server]()
        {
          std::unique_lock lock(g_mutex);
          g_cv.wait(lock, []() { return g_received_signal.load() != 0; });
          server->Shutdown();
        });

    server->Wait();

    if (shutdown_thread.joinable())
    {
      shutdown_thread.join();
    }

    int sig = g_received_signal.load();
    if (sig != 0)
    {
      tools::Logger::getInstance().info("Received signal {}, shutdown complete", sig);
    }
  }
  catch (const std::exception& e)
  {
    tools::Logger::getInstance().error("Unhandled exception in main: {}", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
