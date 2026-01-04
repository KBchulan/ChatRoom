#include "global.hpp"

#include <QCryptographicHash>
#include <QStyle>

std::function<void(QWidget*)> repolish = [](QWidget* widget) -> void
{
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);
};

QString encryPassword(const QString& password)
{
  // 固定盐值，保持明文，不要改动
  constexpr auto kSalt = "ChatRoom@KBchulan#2025!Secure";

  // 组合密码和盐值
  QByteArray saltedPassword = password.toUtf8() + kSalt;

  // SHA-256 哈希
  QByteArray hash = QCryptographicHash::hash(saltedPassword, QCryptographicHash::Sha256);

  return QString::fromLatin1(hash.toHex());
}

QString CHATROOM_API_BASE_URL = "";
