/******************************************************************************
 *
 * @file       db_params.hpp
 * @brief      MySQL prepared statement 参数绑定辅助
 *
 * @author     KBchulan
 * @date       2025/12/04
 * @history
 ******************************************************************************/

#ifndef DB_PARAMS_HPP
#define DB_PARAMS_HPP

#include <mysql/mysql.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utils/UtilsExport.hpp>
#include <vector>

namespace utils
{

// ============================================================================
// 参数绑定 (用于 INSERT/UPDATE/DELETE 的输入参数)
// ============================================================================

struct UTILS_EXPORT ParamHolder
{
  MYSQL_BIND bind{};
  unsigned long length{0};
};

// 字符串类型
inline ParamHolder MakeBind(const std::string& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_STRING;
  holder.bind.buffer = const_cast<char*>(value.data());
  holder.bind.buffer_length = value.size();
  holder.length = value.size();
  holder.bind.length = &holder.length;
  return holder;
}

// 有符号整数类型
inline ParamHolder MakeBind(const std::int8_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_TINY;
  holder.bind.buffer = const_cast<std::int8_t*>(&value);
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ParamHolder MakeBind(const std::int16_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_SHORT;
  holder.bind.buffer = const_cast<std::int16_t*>(&value);
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ParamHolder MakeBind(const std::int32_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONG;
  holder.bind.buffer = const_cast<std::int32_t*>(&value);
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ParamHolder MakeBind(const std::int64_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONGLONG;
  holder.bind.buffer = const_cast<std::int64_t*>(&value);
  holder.bind.is_unsigned = 0;
  return holder;
}

// 无符号整数类型
inline ParamHolder MakeBind(const std::uint8_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_TINY;
  holder.bind.buffer = const_cast<std::uint8_t*>(&value);
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ParamHolder MakeBind(const std::uint16_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_SHORT;
  holder.bind.buffer = const_cast<std::uint16_t*>(&value);
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ParamHolder MakeBind(const std::uint32_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONG;
  holder.bind.buffer = const_cast<std::uint32_t*>(&value);
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ParamHolder MakeBind(const std::uint64_t& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONGLONG;
  holder.bind.buffer = const_cast<std::uint64_t*>(&value);
  holder.bind.is_unsigned = 1;
  return holder;
}

// 浮点类型
inline ParamHolder MakeBind(const float& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_FLOAT;
  holder.bind.buffer = const_cast<float*>(&value);
  return holder;
}

inline ParamHolder MakeBind(const double& value)
{
  ParamHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_DOUBLE;
  holder.bind.buffer = const_cast<double*>(&value);
  return holder;
}

// 变参模板构建参数列表
template <typename... Args>
std::vector<ParamHolder> MakeParams(Args&... args)
{
  std::vector<ParamHolder> holders;
  holders.reserve(sizeof...(Args));
  (holders.push_back(MakeBind(args)), ...);
  return holders;
}

// ============================================================================
// 结果绑定 (用于 SELECT 查询的输出结果)
// ============================================================================

// 固定大小的字符串 buffer
template <std::size_t N>
struct UTILS_EXPORT StringBuffer
{
  std::array<char, N> data{};
  unsigned long length{0};

  // cppcheck-suppress unusedFunction
  [[nodiscard]] std::string str() const
  {
    return std::string(data.data(), length);
  }
};

struct UTILS_EXPORT ResultHolder
{
  MYSQL_BIND bind{};
};

// 字符串 buffer
template <std::size_t N>
inline ResultHolder MakeResultBind(StringBuffer<N>& buf)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_STRING;
  holder.bind.buffer = buf.data.data();
  holder.bind.buffer_length = N;
  holder.bind.length = &buf.length;
  return holder;
}

// 有符号整数类型
inline ResultHolder MakeResultBind(std::int8_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_TINY;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ResultHolder MakeResultBind(std::int16_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_SHORT;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ResultHolder MakeResultBind(std::int32_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONG;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 0;
  return holder;
}

inline ResultHolder MakeResultBind(std::int64_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONGLONG;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 0;
  return holder;
}

// 无符号整数类型
inline ResultHolder MakeResultBind(std::uint8_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_TINY;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ResultHolder MakeResultBind(std::uint16_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_SHORT;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ResultHolder MakeResultBind(std::uint32_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONG;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 1;
  return holder;
}

inline ResultHolder MakeResultBind(std::uint64_t& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_LONGLONG;
  holder.bind.buffer = &value;
  holder.bind.is_unsigned = 1;
  return holder;
}

// 浮点类型
inline ResultHolder MakeResultBind(float& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_FLOAT;
  holder.bind.buffer = &value;
  return holder;
}

inline ResultHolder MakeResultBind(double& value)
{
  ResultHolder holder;
  holder.bind.buffer_type = MYSQL_TYPE_DOUBLE;
  holder.bind.buffer = &value;
  return holder;
}

// 变参模板构建结果列表
template <typename... Args>
// cppcheck-suppress unusedFunction
std::vector<ResultHolder> MakeResults(Args&... args)
{
  std::vector<ResultHolder> holders;
  holders.reserve(sizeof...(Args));
  (holders.push_back(MakeResultBind(args)), ...);
  return holders;
}

}  // namespace utils

#endif  // DB_PARAMS_HPP
