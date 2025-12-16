#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <core/io/io.hpp>
#include <core/logic/logic.hpp>
#include <core/server/server.hpp>
#include <tools/Logger.hpp>
#include <utils/grpc/client/status_server_client.hpp>
#include <utils/pool/mariadb/db_pool.hpp>
#include <utils/pool/redis/redis_pool.hpp>

using namespace global::server;

boost::asio::awaitable<void> signal_handler(boost::asio::signal_set& signals)
{
  auto signal = co_await signals.async_wait(boost::asio::use_awaitable);
  tools::Logger::getInstance().info("Received signal {}, stopping gateway", signal);

  auto& ioc = static_cast<boost::asio::io_context&>(signals.get_executor().context());
  ioc.stop();
  co_return;
}

void init()
{
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
  utils::StatusServerClinet::GetInstance().Init(status_address, STATUS_RPC_CONNECTION_POOL_SIZE);
  utils::DBPool::GetInstance().Init(db_config);
  utils::RedisPool::GetInstance().Init(redis_config);
  core::IO::GetInstance();
  core::Logic::GetInstance();
}

int main()
{
  try
  {
    init();

    boost::asio::io_context ioc;
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    boost::asio::co_spawn(ioc, signal_handler(signals), boost::asio::detached);
    auto server = std::make_shared<core::Server>(ioc, DEFAULT_SERVER_PORT);
    server->Start();
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
