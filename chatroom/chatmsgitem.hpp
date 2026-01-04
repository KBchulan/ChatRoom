/******************************************************************************
 *
 * @file       chatmsgitem.hpp
 * @brief      单个聊天消息项组件，包含头像和气泡
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATMSGITEM_HPP
#define CHATMSGITEM_HPP

#include <QWidget>

#include "global.hpp"

class QLabel;
class QVBoxLayout;
class BubbleBase;
class ChatMsgItem : public QWidget
{
  Q_OBJECT

public:
  explicit ChatMsgItem(MsgDirection direction, QWidget* parent = nullptr);

  void setAvatar(const QPixmap& avatar);
  void setNickname(const QString& name, bool visible = false);
  void setBubble(BubbleBase* bubble);

private:
  void initUI();
  static QPixmap createRoundedAvatar(const QPixmap& src, int size);

  MsgDirection _direction;
  QLabel* _avatar_label;
  QWidget* _content_widget;
  QVBoxLayout* _content_layout;
  QLabel* _nickname_label;
  BubbleBase* _bubble;
};

#endif  // CHATMSGITEM_HPP
