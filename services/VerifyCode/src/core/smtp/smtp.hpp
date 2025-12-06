/******************************************************************************
 *
 * @file       smtp.hpp
 * @brief      基于 curl 库的 SMTP 邮件发送封装
 *
 * @author     KBchulan
 * @date       2025/12/05
 * @history
 ******************************************************************************/

#ifndef SMTP_HPP
#define SMTP_HPP

#include <core/CoreExport.hpp>
#include <string>
#include <string_view>

namespace core
{

// SMTP 邮件发送客户端
class CORE_EXPORT SmtpClient
{
public:
  // 构造函数
  SmtpClient(std::string_view smtp_url, std::string_view username, std::string_view password);

  // 发送验证码邮件
  [[nodiscard]] std::string SendVerifyCode(std::string_view to_email, std::string_view code) const;

private:
  std::string _smtp_url;
  std::string _username;
  std::string _password;
};

}  // namespace core

#endif  // SMTP_HPP
