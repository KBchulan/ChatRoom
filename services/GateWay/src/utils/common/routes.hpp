/******************************************************************************
 *
 * @file       routes.hpp
 * @brief      路由集中声明处
 *
 * @author     KBchulan
 * @date       2025/12/03
 * @history
 ******************************************************************************/

#ifndef ROUTES_HPP
#define ROUTES_HPP

#include <string>
#include <unordered_set>

namespace utils
{
// ================
// user 模块路由
// ================
constexpr const char* HEALTH_CHECK_ROUTE = "/health-check";
constexpr const char* USER_SEND_CODE_ROUTE = "/user/send-code";
constexpr const char* USER_REGISTER_ROUTE = "/user/register";
constexpr const char* USER_RESET_PASS_ROUTE = "/user/reset";

inline const std::unordered_set<std::string> NO_AUTH_ROUTES = {
    HEALTH_CHECK_ROUTE,
    USER_SEND_CODE_ROUTE,
    USER_REGISTER_ROUTE,
    USER_RESET_PASS_ROUTE
};

}  // namespace utils

#endif  // ROUTES_HPP