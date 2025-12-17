#ifndef USERINFO_HPP
#define USERINFO_HPP

#include <QString>

class UserInfo
{
public:
  static UserInfo &GetInstance();

  void SetUUID(const QString &uuid);
  void SetNickname(const QString &nickname);
  void SetEmail(const QString &email);
  void SetAvatar(const QString &avatar);
  void SetJwtToken(const QString &jwt_token);

  [[nodiscard]] const QString &GetUUID() const;
  [[nodiscard]] const QString &GetNickname() const;
  [[nodiscard]] const QString &GetEmail() const;
  [[nodiscard]] const QString &GetAvatar() const;
  [[nodiscard]] const QString &GetJwtToken() const;

  UserInfo(const UserInfo&) = delete;
  UserInfo& operator=(const UserInfo&) = delete;
  UserInfo(UserInfo&&) = delete;
  UserInfo& operator=(UserInfo&&) = delete;

private:
  UserInfo() = default;
  ~UserInfo() = default;

  QString _uuid;
  QString _nickname;
  QString _email;
  QString _avatar;
  QString _jwt_token;
};

#endif  // USERINFO_HPP
