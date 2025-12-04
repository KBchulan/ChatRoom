#include <boost/asio/signal_set.hpp>
#include <core/server/server.hpp>
#include <global/Global.hpp>
#include <tools/Logger.hpp>
#include <utils/grpc/client/verify_code_client.hpp>
#include <utils/pool/db_pool.hpp>

using namespace global::server;

// 初始化连接池
void init_connection_pool()
{
  utils::VerifyCodeClient::GetInstance().Init(std::string(RPC_SERVER_HOST) + ":" + std::to_string(RPC_SERVER_PORT),
                                              RPC_CONNECTION_POOL_SIZE);

  utils::DBConfig config{.host = DB_HOST,
                         .port = DB_PORT,
                         .user = DB_USER,
                         .password = DB_PASSWORD,
                         .database = DB_NAME,
                         .pool_size = DB_MAX_POOL_SIZE};

  utils::DBPool::GetInstance().Init(config);
}

int main()
{
  try
  {
    init_connection_pool();

    boost::asio::io_context ioc;
    const auto& logger = tools::Logger::getInstance();

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    auto signals_handler = [&ioc, &logger](const boost::system::error_code& err, int signal) -> void
    {
      if (!err)
      {
        logger.info("Received signal {}, stopping gateway", signal);
        ioc.stop();
      }
      else
      {
        logger.error("Error receiving signal: {}", err.message());
      }
    };
    signals.async_wait(std::move(signals_handler));

    core::Server server(ioc, DEFAULT_SERVER_PORT);
    ioc.run();
  }
  catch (const boost::system::system_error& e)
  {
    tools::Logger::getInstance().error("Boost system error: {}", e.what());
    return EXIT_FAILURE;
  }
  catch (const std::exception& e)
  {
    tools::Logger::getInstance().error("Standard exception: {}", e.what());
    return EXIT_FAILURE;
  }
  catch (...)
  {
    tools::Logger::getInstance().error("Unknown exception occurred");
    return EXIT_FAILURE;
  }
}