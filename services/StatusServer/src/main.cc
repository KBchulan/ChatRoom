#include <grpcpp/grpcpp.h>

#include <atomic>
#include <condition_variable>
#include <core/server/server.hpp>
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
    using namespace global::server;

    // 创建builder，配置选项
    grpc::ServerBuilder builder;
    builder.AddListeningPort(global::server::SERVER_ADDRESS, grpc::InsecureServerCredentials());

    auto tcp_server1 = core::TcpServerInfo{.host = TCP_ADDRESS_1, .port = TCP_PORT_1};
    auto tcp_server2 = core::TcpServerInfo{.host = TCP_ADDRESS_2, .port = TCP_PORT_2};

    // 注册服务
    core::StatusServiceImpl service({tcp_server1, tcp_server2});
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
