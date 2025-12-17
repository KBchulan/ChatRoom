#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <cstdint>
#include <QString>

class ServerInfo
{
public:
  ServerInfo(QString host, std::uint16_t port, QString token);
  ~ServerInfo();

  [[nodiscard]] QString GetHost() const;
  [[nodiscard]] std::uint16_t GetPort() const;
  [[nodiscard]] QString GetToken() const;

private:
  QString _host;
  std::uint16_t _port;
  QString _token;
};

#endif  // SERVERINFO_HPP
