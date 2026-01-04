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

#define UTILS_API_V1_PREFIX "/api/v1"
#define UTILS_ROUTE(path) UTILS_API_V1_PREFIX path

// ================
// user 模块路由
// ================
constexpr const char* HEALTH_CHECK_ROUTE = UTILS_ROUTE("/health-check");
constexpr const char* USER_SEND_CODE_ROUTE = UTILS_ROUTE("/user/send-code");
constexpr const char* USER_REGISTER_ROUTE = UTILS_ROUTE("/user/register");
constexpr const char* USER_RESET_PASS_ROUTE = UTILS_ROUTE("/user/reset");
constexpr const char* USER_LOGIN_ROUTE = UTILS_ROUTE("/user/login");

// 用于 JWT 校验
inline const std::unordered_set<std::string> NO_AUTH_ROUTES = {
    HEALTH_CHECK_ROUTE, USER_SEND_CODE_ROUTE, USER_REGISTER_ROUTE, USER_RESET_PASS_ROUTE, USER_LOGIN_ROUTE};

// 用于服务降级使用
inline const std::unordered_set<std::string> AVAILABLE_ROUTES = {
    HEALTH_CHECK_ROUTE, USER_SEND_CODE_ROUTE, USER_REGISTER_ROUTE, USER_RESET_PASS_ROUTE, USER_LOGIN_ROUTE};

}  // namespace utils

#endif  // ROUTES_HPP