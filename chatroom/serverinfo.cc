#include "serverinfo.hpp"

ServerInfo::ServerInfo(QString uuid, QString host, std::int16_t port, QString token)
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

std::int16_t ServerInfo::GetPort() const
{
    return _port;
}

QString ServerInfo::GetToken() const
{
    return _token;
}
