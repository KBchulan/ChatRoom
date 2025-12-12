#include <boost/asio/signal_set.hpp>
#include <core/business/business.hpp>
#include <core/io/io.hpp>
#include <core/server/server.hpp>
#include <global/Global.hpp>
#include <tools/Logger.hpp>
#include <utils/grpc/client/status_server_client.hpp>
#include <utils/grpc/client/verify_code_client.hpp>
#include <utils/pool/mariadb/db_pool.hpp>
#include <utils/pool/redis/redis_pool.hpp>

using namespace global::server;

// 初始化连接池
void init()
{
  std::string email_rpc_address = std::string(EMAIL_RPC_SERVER_HOST) + ":" + std::to_string(EMAIL_RPC_SERVER_PORT);
  std::string status_address = std::string(STATUS_RPC_SERVER_HOST) + ":" + std::to_string(STATUS_RPC_SERVER_PORT);

  utils::DBConfig db_config{.host = DB_HOST,
                            .port = DB_PORT,
                            .user = DB_USER,
                            .password = DB_PASSWORD,
                            .database = DB_NAME,
                            .pool_size = DB_MAX_POOL_SIZE};

  utils::RedisConfig redis_config{.host = REDIS_HOST,
                                  .port = REDIS_PORT,
                                  .password = REDIS_PASSWORD,
                                  .db_index = REDIS_DB_INDEX,
                                  .pool_size = REDIS_MAX_POOL_SIZE,
                                  .timeout = std::chrono::seconds(REDIS_TIMEOUT)};

  tools::Logger::getInstance();
  utils::VerifyCodeClient::GetInstance().Init(email_rpc_address, EMAIL_RPC_CONNECTION_POOL_SIZE);
  utils::StatusServerClinet::GetInstance().Init(status_address, STATUS_RPC_CONNECTION_POOL_SIZE);
  utils::DBPool::GetInstance().Init(db_config);
  utils::RedisPool::GetInstance().Init(redis_config);
  core::Business::GetInstance();
  core::IO::GetInstance();
}

int main()
{
  try
  {
    init();

    boost::asio::io_context ioc;
    const auto& logger = tools::Logger::getInstance();

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    auto signals_handler = [&ioc, &logger](const boost::system::error_code& err, int signal) -> void
    {
      if (!err)
      {
        std::cout << '\n';
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