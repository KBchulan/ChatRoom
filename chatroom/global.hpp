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
extern QString encryPassword(const QString& password);
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

// 消息方向
enum class MsgDirection : std::uint8_t
{
  Sent = 0,      // 自己发送
  Received = 1,  // 收到的消息
};

// 联系人分组
enum class ContactGroup : std::uint8_t
{
  NewFriends = 0,  // 新的朋友
  A,               // 字母组
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  Hash,  // # 组
  Count  // 分组总数
};

namespace UIConstants
{
// mainwindow.hpp
constexpr int WindowLoginWidth = 380;   // 登录窗口宽度
constexpr int WindowLoginHeight = 570;  // 登录窗口高度
constexpr int WindowChatWidth = 1080;   // 聊天窗口宽度
constexpr int WindowChatHeight = 720;   // 聊天窗口高度

// chatdialog.hpp
constexpr int ChatUserItemHeight = 65;      // 联系人列表项高度
constexpr int SearchUserItemHeight = 65;    // 搜索用户列表项高度
constexpr int LoadingItemHeight = 65;       // 加载中列表项高度
constexpr int ContactUserItemHeight = 65;   // 联系人列表项高度
constexpr int ContactGroupItemHeight = 25;  // 联系人分组项高度
constexpr int FriendApplyItemHeight = 70;   // 好友申请列表项高度

// chatmsgitem.hpp
constexpr int ChatMsgItemAvatarSize = 50;    // 头像大小
constexpr int ChatMsgItemTriangleWidth = 8;  // 小三角宽度
constexpr int ChatMsgItemBubbleRadius = 6;   // 气泡圆角半径
}  // namespace UIConstants

// 拼音相关的操作
namespace PinyinUtils
{
constexpr ushort kChineseStart = 0x4E00;
constexpr ushort kChineseEnd = 0x9FA5;

extern const std::string_view kPinyinDict;

char GetFirstPinyinLetter(QChar cha);
}  // namespace PinyinUtils

#endif  // GLOBAL_HPP
