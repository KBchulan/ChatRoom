#include "serverinfo.hpp"

ServerInfo::ServerInfo(QString uuid, QString host, std::uint16_t port, QString token)
    : _uuid(uuid), _host(host), _port(port), _token(token)
{
}

ServerInfo::~ServerInfo() = default;

QString ServerInfo::GetUUID() const
{
    return _uuid;
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
