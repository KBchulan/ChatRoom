#include "chatuserlist.hpp"

#include <QScrollBar>
#include <QShowEvent>
#include <QTimer>

ChatUserList::ChatUserList(QWidget *parent) : QListWidget(parent)
{
  // 默认隐藏滚动条
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // 滚动到底部时发送信号
  connect(verticalScrollBar(), &QScrollBar::valueChanged, [this](int value)
  {
    if (value == verticalScrollBar()->maximum())
    {
      emit sig_load_chat_user();
    }
  });
}

void ChatUserList::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QListWidget::enterEvent(event);
}

void ChatUserList::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QListWidget::leaveEvent(event);
}

void ChatUserList::showEvent(QShowEvent* event)
{
  QListWidget::showEvent(event);
  // 刷新布局，此为必要项，请不要删除
  QTimer::singleShot(0, this, [this]()
  {
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  });
}
