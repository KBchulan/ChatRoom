#include "smtp.hpp"

#include <curl/curl.h>

#include <cstring>
#include <sstream>

namespace core
{

struct UploadContext
{
  std::size_t bytes_read = 0;
  std::string payload;
};

auto generate_verify_code_html(std::string_view code) -> std::string
{
  std::ostringstream oss;
  oss << R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f4; padding: 20px; }
        .container { max-width: 600px; margin: 0 auto; background: #fff; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #333; }
        .code { font-size: 32px; font-weight: bold; color: #007bff; text-align: center; padding: 20px; background: #f8f9fa; border-radius: 5px; letter-spacing: 5px; margin: 20px 0; }
        .footer { text-align: center; color: #999; font-size: 12px; margin-top: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h2 class="header">)"
      << "邮箱验证码" << R"(</h2>
        <p>)"
      << "您好，您正在进行邮箱验证操作，验证码为：" << R"(</p>
        <div class="code">)"
      << code << R"(</div>
        <p>)"
      << "验证码有效期为 1 分钟，请勿将验证码泄露给他人。" << R"(</p>
        <p>)"
      << "如果这不是您本人的操作，请忽略此邮件。" << R"(</p>
        <div class="footer">
            <p>)"
      << "此邮件由系统自动发送，请勿直接回复。" << R"(</p>
        </div>
    </div>
</body>
</html>)";

  return oss.str();
}

auto build_payload(std::string_view from, std::string_view to_email, std::string_view body) -> std::string
{
  std::ostringstream oss;
  oss << "To: " << to_email << "\r\n";
  oss << "From: " << from << "\r\n";
  oss << "Subject: =?UTF-8?B?6aqM6K+B56CB?=\r\n";
  oss << "Content-Type: text/html; charset=UTF-8\r\n";
  oss << "\r\n";
  oss << body << "\r\n";

  return oss.str();
}

auto payload_source(char* ptr, std::size_t size, std::size_t nmemb, void* userp) -> std::size_t
{
  auto* ctx = static_cast<UploadContext*>(userp);
  const std::size_t buffer_size = size * nmemb;

  if (ctx->bytes_read >= ctx->payload.size())
  {
    return 0;
  }

  std::size_t remaining = ctx->payload.size() - ctx->bytes_read;
  std::size_t copy_size = std::min(remaining, buffer_size);

  std::memcpy(ptr, ctx->payload.c_str() + ctx->bytes_read, copy_size);
  ctx->bytes_read += copy_size;

  return copy_size;
}

SmtpClient::SmtpClient(std::string_view smtp_url, std::string_view username, std::string_view password)
    : _smtp_url(smtp_url), _username(username), _password(password)
{
}

std::string SmtpClient::SendVerifyCode(std::string_view to_email, std::string_view code) const
{
  CURL* curl = curl_easy_init();
  if (curl == nullptr)
  {
    return "curl init failed";
  }

  std::string body = generate_verify_code_html(code);
  UploadContext ctx{.bytes_read = 0, .payload = build_payload(_username, to_email, body)};

  curl_slist* recipients = nullptr;
  std::string to_addr = "<" + std::string(to_email) + ">";
  std::string from_addr = "<" + _username + ">";
  recipients = curl_slist_append(recipients, to_addr.c_str());

  curl_easy_setopt(curl, CURLOPT_URL, _smtp_url.c_str());
  curl_easy_setopt(curl, CURLOPT_USERNAME, _username.c_str());
  curl_easy_setopt(curl, CURLOPT_PASSWORD, _password.c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_addr.c_str());
  curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
  curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");
  curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
  curl_easy_setopt(curl, CURLOPT_READDATA, &ctx);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

  CURLcode res = curl_easy_perform(curl);

  curl_slist_free_all(recipients);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK)
  {
    std::string err_msg = "send email failed: ";
    err_msg += curl_easy_strerror(res);
    return err_msg;
  }

  return "";
}

}  // namespace core
