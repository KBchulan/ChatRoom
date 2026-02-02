#include "chatmsglist.hpp"

#include <QScrollBar>

#include "chatmsgitem.hpp"

ChatMsgList::ChatMsgList(QWidget* parent) : CustomListWidget(parent)
{
  setSelectionMode(QAbstractItemView::NoSelection);

  _debounce_timer = new QTimer(this);
  _debounce_timer->setSingleShot(true);
  _debounce_timer->setInterval(200);

  connect(_debounce_timer, &QTimer::timeout, this, [this]() { emit sig_load_more_msg(); });

  // 滚动到顶部时发送信号，加载历史消息
  connect(verticalScrollBar(), &QScrollBar::valueChanged,
          [this](int value)
          {
            if (value == verticalScrollBar()->minimum() && !_debounce_timer->isActive())
            {
              _debounce_timer->start();
            }
          });
}

void ChatMsgList::mousePressEvent(QMouseEvent* event)
{
  emit sig_clicked();
  QListWidget::mousePressEvent(event);
}

void ChatMsgList::appendChatItem(ChatMsgItem* item)
{
  auto* list_item = new QListWidgetItem(this);
  list_item->setSizeHint(item->sizeHint());
  addItem(list_item);
  setItemWidget(list_item, item);
  scrollToBottom();
}
