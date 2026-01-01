/******************************************************************************
 *
 * @file       chatmsgitem.hpp
 * @brief      单个聊天信息气泡组件
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATMSGITEM_HPP
#define CHATMSGITEM_HPP

#include <QWidget>

namespace Ui
{
class ChatMsgItem;
}

class ChatMsgItem : public QWidget
{
  Q_OBJECT

public:
  explicit ChatMsgItem(QWidget* parent = nullptr);
  ~ChatMsgItem();

private:
  Ui::ChatMsgItem* ui;
};

#endif  // CHATMSGITEM_HPP
