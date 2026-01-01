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
#include <cstdint>
#include <functional>

extern std::function<void(QWidget*)> repolish;
extern QString CHATROOM_API_BASE_URL;

enum class ReqID : std::uint16_t
{
  ID_GET_VERIFY_CODE = 1001,      // 获取验证码
  ID_REGISTER = 1002,             // 注册
  ID_RESET_PASSWORD = 1003,       // 重置密码
  ID_LOGIN = 1004,                // 登录
  ID_LOGIN_CHAT = 1005,           // 逻辑登录
  ID_LOGIN_CHAT_RESPONSE = 1006,  // 登录回包
};

enum class Module : std::uint8_t
{
  REGISTER = 0,  // 注册模块
  RESET = 1,     // 重置密码模块
  LOGIN = 2,     // 登录模块
};

enum class ErrorCode : std::uint8_t
{
  SUCCESS = 0,        // 成功
  NETWORK_ERROR = 1,  // 网络错误
};

namespace UIConstants
{
// mainwindow.hpp
constexpr int WindowLoginWidth = 380;
constexpr int WindowLoginHeight = 570;
constexpr int WindowChatWidth = 1080;
constexpr int WindowChatHeight = 720;

// chatdialog.hpp
constexpr int ChatUserItemHeight = 65;
constexpr int LoadingItemHeight = 65;
}  // namespace UIConstants

#endif  // GLOBAL_HPP
