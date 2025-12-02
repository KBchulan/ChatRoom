/******************************************************************************
 *
 * @file       query_parser.hpp
 * @brief      解析 Query 参数
 *
 * @author     KBchulan
 * @date       2025/12/02
 * @history
 ******************************************************************************/

#ifndef QUERY_PARSER_HPP
#define QUERY_PARSER_HPP

#include <string>

namespace utils
{

template <typename T>
concept Parsable = requires(const std::string& str) {
  { T::FromQueryString(str) } -> std::same_as<T>;
};

template <Parsable QueryType>
QueryType ParseQuery(const std::string& target)
{
  auto pos = target.find('?');
  std::string query_str = (pos == std::string::npos) ? "" : target.substr(pos + 1);
  return QueryType::FromQueryString(query_str);
}

}  // namespace utils

#endif  // QUERY_PARSER_HPP