#include "global.hpp"

#include <QStyle>

std::function<void(QWidget*)> repolish = [](QWidget* w) -> void
{
  w->style()->unpolish(w);
  w->style()->polish(w);
};

QString CHATROOM_API_BASE_URL = "";
