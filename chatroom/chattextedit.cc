#include "chattextedit.hpp"

ChatTextEdit::ChatTextEdit(QWidget* parent) : QTextEdit(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ChatTextEdit::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QTextEdit::enterEvent(event);
}

void ChatTextEdit::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QTextEdit::leaveEvent(event);
}
