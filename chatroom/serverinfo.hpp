/******************************************************************************
 *
 * @file       serverinfo.hpp
 * @brief      存储聊天使用 Tcp 服务器信息
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <QString>
#include <cstdint>

class ServerInfo
{
public:
  ServerInfo(QString host, std::uint16_t port, QString token);
  ~ServerInfo();

  ServerInfo(const ServerInfo&) noexcept;
  ServerInfo& operator=(const ServerInfo&) noexcept;
  ServerInfo(ServerInfo&& other) noexcept;
  ServerInfo& operator=(ServerInfo&& other) noexcept;

  [[nodiscard]] QString GetHost() const;
  [[nodiscard]] std::uint16_t GetPort() const;
  [[nodiscard]] QString GetToken() const;

private:
  QString _host;
  std::uint16_t _port;
  QString _token;
};

#endif  // SERVERINFO_HPP
