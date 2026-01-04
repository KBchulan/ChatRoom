#include "chatmsgitem.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include "bubblebase.hpp"

ChatMsgItem::ChatMsgItem(MsgDirection direction, QWidget* parent)
    : QWidget(parent), _direction(direction), _bubble(nullptr)
{
  initUI();
}

void ChatMsgItem::initUI()
{
  const int avatar_size = UIConstants::ChatMsgItemAvatarSize;

  auto* main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(10, 5, 10, 5);
  main_layout->setSpacing(8);

  // 头像
  _avatar_label = new QLabel(this);
  _avatar_label->setObjectName("msg_avatar");
  _avatar_label->setFixedSize(avatar_size, avatar_size);
  _avatar_label->setAlignment(Qt::AlignTop);

  // 内容区: 昵称 + 气泡，昵称默认隐藏
  _content_widget = new QWidget(this);
  _content_layout = new QVBoxLayout(_content_widget);
  _content_layout->setContentsMargins(0, 0, 0, 0);
  _content_layout->setSpacing(2);

  // 昵称
  _nickname_label = new QLabel(this);
  _nickname_label->setObjectName("msg_nickname");
  _nickname_label->setVisible(false);

  _content_layout->addWidget(_nickname_label);

  // 根据方向布局
  if (_direction == MsgDirection::Sent)
  {
    // 自己发送：弹簧 | 内容 | 头像
    _nickname_label->setAlignment(Qt::AlignRight);
    main_layout->addStretch();
    main_layout->addWidget(_content_widget);
    main_layout->addWidget(_avatar_label, 0, Qt::AlignTop);
  }
  else
  {
    // 对方消息：头像 | 内容 | 弹簧
    _nickname_label->setAlignment(Qt::AlignLeft);
    main_layout->addWidget(_avatar_label, 0, Qt::AlignTop);
    main_layout->addWidget(_content_widget);
    main_layout->addStretch();
  }
}

void ChatMsgItem::setAvatar(const QPixmap& avatar)
{
  const int size = UIConstants::ChatMsgItemAvatarSize;
  QPixmap rounded = createRoundedAvatar(avatar, size);
  _avatar_label->setPixmap(rounded);
}

void ChatMsgItem::setBubble(BubbleBase* bubble)
{
  if (_bubble != nullptr)
  {
    _content_layout->removeWidget(_bubble);
    delete _bubble;
  }
  _bubble = bubble;
  _bubble->setParent(this);
  _content_layout->insertWidget(1, _bubble);
}

void ChatMsgItem::setNickname(const QString& name, bool visible)
{
  _nickname_label->setText(name);
  _nickname_label->setVisible(visible);
}

QPixmap ChatMsgItem::createRoundedAvatar(const QPixmap& src, int size)
{
  QPixmap scaled = src.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

  QPixmap result(size, size);
  result.fill(Qt::transparent);

  QPainter painter(&result);
  painter.setRenderHint(QPainter::Antialiasing);

  QPainterPath path;
  path.addEllipse(0, 0, size, size);
  painter.setClipPath(path);
  painter.drawPixmap(0, 0, scaled);

  return result;
}
