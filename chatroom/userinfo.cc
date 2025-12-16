#include "userinfo.hpp"

UserInfo &UserInfo::GetInstance()
{
  static UserInfo instance;
  return instance;
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
