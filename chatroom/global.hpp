/******************************************************************************
 *
 * @file       global.hpp
 * @brief      全局定义文件
 *
 * @author     KBchulan
 * @date       2025/12/01
 * @history
 *****************************************************************************/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QString>
#include <QWidget>
#include <functional>

extern std::function<void(QWidget*)> repolish;
extern QString GateWayUrl;

enum class ReqID
{
  ID_GET_VERIFY_CODE = 1001,       // 获取验证码
  ID_REGISTER = 1002,              // 注册
  ID_RESET_PASSWORD = 1003,        // 重置密码
  ID_LOGIN = 1004,                 // 登录
  ID_LOGIN_CHAT = 1005,            // 逻辑登录
  ID_LOGIN_CHAT_RESPONSE = 1006,   // 登录回包
};

enum class Module
{
  REGISTER = 0,  // 注册模块
  RESET = 1,     // 重置密码模块
  LOGIN = 2,     // 登录模块
};

enum class ErrorCode
{
  SUCCESS = 0,           // 成功
  NETWORK_ERROR = 1,     // 网络错误
};

#endif  // GLOBAL_HPP
