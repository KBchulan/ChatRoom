#include "chatmsglist.hpp"

#include <QScrollBar>

ChatMsgList::ChatMsgList(QWidget* parent) : QListWidget(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  _debounce_timer = new QTimer(this);
  _debounce_timer->setSingleShot(true);
  _debounce_timer->setInterval(200);

  connect(_debounce_timer, &QTimer::timeout, this, [this]() { emit sig_load_more_msg(); });

  // 滚动到顶部时发送信号，加载历史消息
  connect(verticalScrollBar(), &QScrollBar::valueChanged,
          [this](int value)
          {
            if (value == verticalScrollBar()->minimum())
            {
              if (!_debounce_timer->isActive())
              {
                _debounce_timer->start();
              }
            }
          });
}

void ChatMsgList::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QListWidget::enterEvent(event);
}

void ChatMsgList::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QListWidget::leaveEvent(event);
}

void ChatMsgList::showEvent(QShowEvent* event)
{
  QListWidget::showEvent(event);
  // 刷新布局，此为必要项，不要删除
  QTimer::singleShot(0, this,
                     [this]()
                     {
                       setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                       setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                     });
}
