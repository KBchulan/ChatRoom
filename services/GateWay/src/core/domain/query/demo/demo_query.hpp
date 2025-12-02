/******************************************************************************
 *
 * @file       demo_query.hpp
 * @brief      demo 模块的 query 请求
 *
 * @author     KBchulan
 * @date       2025/12/02
 * @history
 ******************************************************************************/

#ifndef DEMO_QUERY_HPP
#define DEMO_QUERY_HPP

#include <string>

namespace core
{

struct GetTestQuery
{
  std::string name;
  int age{0};

  static GetTestQuery FromQueryString(const std::string& query_string)
  {
    GetTestQuery query{};
    std::string_view str_view = query_string;

    while (!str_view.empty())
    {
      auto amp_pos = str_view.find('&');
      auto pair = str_view.substr(0, amp_pos);

      if (auto eq_pos = pair.find('='); eq_pos != std::string_view::npos)
      {
        auto key = pair.substr(0, eq_pos);
        auto value = pair.substr(eq_pos + 1);

        if (key == "name")
        {
          query.name = value;
        }
        else if (key == "age")
        {
          query.age = std::stoi(std::string(value));
        }
      }

      if (amp_pos == std::string_view::npos)
      {
        break;
      }
      str_view = str_view.substr(amp_pos + 1);
    }

    return query;
  }
};

}  // namespace core

#endif  // DEMO_QUERY_HPP