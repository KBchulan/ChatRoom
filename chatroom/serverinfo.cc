#include "serverinfo.hpp"

ServerInfo::ServerInfo(QString host, std::uint16_t port, QString token)
    : _host(host), _port(port), _token(token)
{
}

ServerInfo::~ServerInfo() = default;

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
