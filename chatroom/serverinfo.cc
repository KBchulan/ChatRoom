#include "serverinfo.hpp"

ServerInfo::ServerInfo(QString host, std::uint16_t port, QString token)
    : _host(std::move(host)), _port(port), _token(std::move(token))
{
}

ServerInfo::~ServerInfo() = default;

ServerInfo::ServerInfo(const ServerInfo& other) noexcept = default;

ServerInfo& ServerInfo::operator=(const ServerInfo& other) noexcept
{
  if (this != &other)
  {
    _host = other._host;
    _port = other._port;
    _token = other._token;
  }
  return *this;
}

ServerInfo::ServerInfo(ServerInfo&& other) noexcept
    : _host(std::move(other._host)), _port(other._port), _token(std::move(other._token))
{
}

ServerInfo& ServerInfo::operator=(ServerInfo&& other) noexcept
{
  if (this != &other)
  {
    _host = std::move(other._host);
    _port = other._port;
    _token = std::move(other._token);
  }
  return *this;
}

QString ServerInfo::GetHost() const
{
  return _host;
}

std::uint16_t ServerInfo::GetPort() const
{
  return _port;
}

QString ServerInfo::GetToken() const
{
  return _token;
}
