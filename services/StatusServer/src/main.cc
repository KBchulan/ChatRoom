#include <grpcpp/grpcpp.h>

#include <atomic>
#include <condition_variable>
#include <core/server/server.hpp>
#include <csignal>
#include <global/Global.hpp>
#include <mutex>
#include <thread>
#include <tools/Cmd.hpp>
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

// 打印使用说明
void print_usage(const char* program_name)
{
  std::cout << "Usage: " << program_name << " [-h] [-p <port>]\n"
            << "Options:\n"
            << "  -h, --help         Show this help message\n"
            << "  -p, --port <port>  Server port (default: " << global::server::DEFAULT_SERVER_PORT << ")\n";
}

// 解析命令行参数
std::optional<tools::CmdOptions> parse_cmd(std::span<char*> args)
{
  tools::CmdOptions options;

  for (std::size_t i = 1; i < args.size(); ++i)
  {
    if (std::strcmp(args[i], "-h") == 0 || std::strcmp(args[i], "--help") == 0)
    {
      options.show_help = true;
      return options;
    }

    if (std::strcmp(args[i], "-p") == 0 || std::strcmp(args[i], "--port") == 0)
    {
      if (i + 1 >= args.size())
      {
        std::cerr << "Error: missing port value\n";
        return std::nullopt;
      }

      try
      {
        int port = std::stoi(args[++i]);
        if (port < 1 || port > 65535)
        {
          std::cerr << "Error: port must be between 1 and 65535\n";
          return std::nullopt;
        }
        options.port = static_cast<unsigned short>(port);
      }
      catch (const std::exception&)
      {
        std::cerr << "Error: invalid port number '" << args[i] << "'\n";
        return std::nullopt;
      }
    }
  }

  return options;
}

}  // namespace

int main(int argc, char* argv[])
{
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  auto options = parse_cmd(std::span(argv, static_cast<std::size_t>(argc)));

  if (!options)
  {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (options->show_help)
  {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  try
  {
    using namespace global::server;
    auto server_addr = std::string(SERVER_ADDRESS) + ":" + std::to_string(options->port);

    // 创建builder，配置选项
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());

    auto tcp_server1 = core::TcpServerInfo{.host = TCP_ADDRESS_1, .port = TCP_PORT_1};
    auto tcp_server2 = core::TcpServerInfo{.host = TCP_ADDRESS_2, .port = TCP_PORT_2};

    // 注册服务
    core::StatusServiceImpl service({tcp_server1, tcp_server2});
    builder.RegisterService(&service);

    // 创建服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    tools::Logger::getInstance().info("Status Server listening on {}", server_addr);

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
