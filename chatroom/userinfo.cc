#include "userinfo.hpp"

UserInfo &UserInfo::GetInstance()
{
  static UserInfo instance;
  return instance;
}

void UserInfo::SetUUID(const QString &uuid)
{
  _uuid = uuid;
}

void UserInfo::SetNickname(const QString& nickname)
{
  _nickname = nickname;
}

void UserInfo::SetEmail(const QString& email)
{
  _email = email;
}

void UserInfo::SetAvatar(const QString& avatar)
{
  _avatar = avatar;
}

void UserInfo::SetJwtToken(const QString &jwt_token)
{
  _jwt_token = jwt_token;
}

const QString& UserInfo::GetUUID() const
{
  return _uuid;
}

const QString& UserInfo::GetNickname() const
{
  return _nickname;
}

const QString& UserInfo::GetEmail() const
{
  return _email;
}

const QString& UserInfo::GetAvatar() const
{
  return _avatar;
}

const QString& UserInfo::GetJwtToken() const
{
  return _jwt_token;
}
