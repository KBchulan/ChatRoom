/******************************************************************************
 *
 * @file       db_pool.hpp
 * @brief      MariaDB/Mysql 的连接池子
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef DB_POOL_HPP
#define DB_POOL_HPP

#include <mysql/mysql.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <global/Global.hpp>
#include <string>
#include <utils/pool/db_params.hpp>
#include <vector>

namespace utils
{

struct DBConfig
{
  std::string host;
  std::uint16_t port;
  std::string user;
  std::string password;
  std::string database;
  std::size_t pool_size;
};

class DBPool;
class PooledConnection
{
public:
  PooledConnection(MYSQL* conn, DBPool* pool, std::size_t slot) : _conn(conn), _pool(pool), _slot(slot)
  {
  }

  ~PooledConnection();

  [[nodiscard]] MYSQL* GetConnection() const
  {
    return _conn;
  }

  // 执行 SQL 语句 (INSERT/UPDATE/DELETE)
  bool Execute(const char* sql, std::vector<ParamHolder>& params)
  {
    MYSQL_STMT* stmt = mysql_stmt_init(_conn);
    if (stmt == nullptr)
    {
      return false;
    }

    if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0)
    {
      mysql_stmt_close(stmt);
      return false;
    }

    // 提取 MYSQL_BIND 数组
    if (!params.empty())
    {
      std::vector<MYSQL_BIND> binds;
      binds.reserve(params.size());
      for (auto& holder : params)
      {
        binds.push_back(holder.bind);
      }

      if (mysql_stmt_bind_param(stmt, binds.data()) != 0)
      {
        mysql_stmt_close(stmt);
        return false;
      }
    }

    int result = mysql_stmt_execute(stmt);
    mysql_stmt_close(stmt);
    return result == 0;
  }

  // 查询单行 (SELECT ... LIMIT 1)，成功返回 true
  bool QueryOne(const char* sql, std::vector<ParamHolder>& params, std::vector<ResultHolder>& results)
  {
    MYSQL_STMT* stmt = mysql_stmt_init(_conn);
    if (stmt == nullptr)
    {
      return false;
    }

    if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0)
    {
      mysql_stmt_close(stmt);
      return false;
    }

    // 绑定输入参数
    if (!params.empty())
    {
      std::vector<MYSQL_BIND> binds;
      binds.reserve(params.size());
      for (auto& holder : params)
      {
        binds.push_back(holder.bind);
      }

      if (mysql_stmt_bind_param(stmt, binds.data()) != 0)
      {
        mysql_stmt_close(stmt);
        return false;
      }
    }

    if (mysql_stmt_execute(stmt) != 0)
    {
      mysql_stmt_close(stmt);
      return false;
    }

    // 绑定输出结果
    if (!results.empty())
    {
      std::vector<MYSQL_BIND> binds;
      binds.reserve(results.size());
      for (auto& holder : results)
      {
        binds.push_back(holder.bind);
      }

      if (mysql_stmt_bind_result(stmt, binds.data()) != 0)
      {
        mysql_stmt_close(stmt);
        return false;
      }
    }

    bool success = (mysql_stmt_fetch(stmt) == 0);
    mysql_stmt_close(stmt);
    return success;
  }

  // 查询多行，返回行数
  template <typename Callback>
  std::size_t QueryMany(const char* sql, std::vector<ParamHolder>& params, std::vector<ResultHolder>& results,
                        Callback callback)
  {
    MYSQL_STMT* stmt = mysql_stmt_init(_conn);
    if (stmt == nullptr)
    {
      return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, std::strlen(sql)) != 0)
    {
      mysql_stmt_close(stmt);
      return 0;
    }

    // 绑定输入参数
    if (!params.empty())
    {
      std::vector<MYSQL_BIND> binds;
      binds.reserve(params.size());
      for (auto& holder : params)
      {
        binds.push_back(holder.bind);
      }

      if (mysql_stmt_bind_param(stmt, binds.data()) != 0)
      {
        mysql_stmt_close(stmt);
        return 0;
      }
    }

    if (mysql_stmt_execute(stmt) != 0)
    {
      mysql_stmt_close(stmt);
      return 0;
    }

    // 绑定输出结果
    if (!results.empty())
    {
      std::vector<MYSQL_BIND> binds;
      binds.reserve(results.size());
      for (auto& holder : results)
      {
        binds.push_back(holder.bind);
      }

      if (mysql_stmt_bind_result(stmt, binds.data()) != 0)
      {
        mysql_stmt_close(stmt);
        return 0;
      }
    }

    std::size_t count = 0;
    while (mysql_stmt_fetch(stmt) == 0)
    {
      callback();
      ++count;
    }

    mysql_stmt_close(stmt);
    return count;
  }

  PooledConnection(const PooledConnection&) = delete;
  PooledConnection& operator=(const PooledConnection&) = delete;
  PooledConnection& operator=(PooledConnection&&) noexcept = delete;

  PooledConnection(PooledConnection&& other) noexcept : _conn(other._conn), _pool(other._pool), _slot(other._slot)
  {
    other._conn = nullptr;
  }

private:
  MYSQL* _conn;
  DBPool* _pool;
  std::size_t _slot;
};

class DBPool
{
  static constexpr std::size_t MAX_POOL_SIZE = global::server::DB_MAX_POOL_SIZE;

  struct Slot
  {
    MYSQL* _conn = nullptr;
    std::atomic<bool> _in_use{false};
  };

public:
  static DBPool& GetInstance()
  {
    static DBPool instance;
    return instance;
  }

  void Init(const DBConfig& config)
  {
    _config = config;
    _pool_size = std::min(config.pool_size, MAX_POOL_SIZE);

    for (std::size_t i = 0; i < _pool_size; ++i)
    {
      _slots[i]._conn = create_connection();
      _slots[i]._in_use.store(false, std::memory_order_relaxed);
    }
    _available.store(_pool_size, std::memory_order_release);
  }

  [[nodiscard]] PooledConnection GetConnection()
  {
    while (true)
    {
      for (std::size_t i = 0; i < _pool_size; ++i)
      {
        bool expected = false;
        if (_slots[i]._in_use.compare_exchange_strong(expected, true, std::memory_order_acquire,
                                                      std::memory_order_relaxed))
        {
          _available.fetch_sub(1, std::memory_order_acquire);

          // 失效就重新连接
          if (mysql_ping(_slots[i]._conn) != 0)
          {
            mysql_close(_slots[i]._conn);
            _slots[i]._conn = create_connection();
          }

          return {_slots[i]._conn, this, i};
        }
      }

      _available.wait(0, std::memory_order_relaxed);
    }
  }

  void ReleaseConnection(std::size_t slot)
  {
    _slots[slot]._in_use.store(false, std::memory_order_release);
    _available.fetch_add(1, std::memory_order_release);
    _available.notify_one();
  }

  ~DBPool()
  {
    for (std::size_t i = 0; i < _pool_size; ++i)
    {
      if (_slots[i]._conn != nullptr)
      {
        mysql_close(_slots[i]._conn);
      }
    }
  }

private:
  DBPool() = default;

  [[nodiscard]] MYSQL* create_connection() const
  {
    MYSQL* conn = mysql_init(nullptr);
    if (conn == nullptr)
    {
      throw std::runtime_error("Failed to initialize MySQL connection");
    }

    unsigned int timeout = 5;
    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    if (mysql_real_connect(conn, _config.host.c_str(), _config.user.c_str(), _config.password.c_str(),
                           _config.database.c_str(), _config.port, nullptr, 0) == nullptr)
    {
      std::string err_msg = mysql_error(conn);
      mysql_close(conn);
      throw std::runtime_error("Failed to connect to MySQL: " + err_msg);
    }

    mysql_set_character_set(conn, "utf8mb4");
    return conn;
  }

  DBConfig _config;
  std::size_t _pool_size{0};
  std::array<Slot, MAX_POOL_SIZE> _slots;
  std::atomic<std::size_t> _available{0};
};

inline PooledConnection::~PooledConnection()
{
  if (_conn != nullptr && _pool != nullptr)
  {
    _pool->ReleaseConnection(_slot);
  }
}

}  // namespace utils

#endif  // DB_POOL_HPP