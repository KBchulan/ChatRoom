#include "global.hpp"

#include <QStyle>

std::function<void(QWidget*)> repolish = [](QWidget* widget) -> void
{
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);
};

QString CHATROOM_API_BASE_URL = "";
