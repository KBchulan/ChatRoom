#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <cstdint>
#include <QString>

class ServerInfo
{
public:
  ServerInfo(QString uuid, QString host, std::int16_t port, QString token);
  ~ServerInfo();

  [[nodiscard]] QString GetUUID() const;
  [[nodiscard]] QString GetHost() const;
  [[nodiscard]] std::int16_t GetPort() const;
  [[nodiscard]] QString GetToken() const;

private:
  QString _uuid;
  QString _host;
  std::int16_t _port;
  QString _token;
};

#endif  // SERVERINFO_HPP
