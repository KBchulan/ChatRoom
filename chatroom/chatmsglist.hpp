/******************************************************************************
 *
 * @file       chatmsglist.hpp
 * @brief      聊天信息列表，自定义QListWidget，用于显示聊天消息
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATMSGLIST_HPP
#define CHATMSGLIST_HPP

#include <QEnterEvent>
#include <QEvent>
#include <QListWidget>
#include <QMouseEvent>
#include <QShowEvent>
#include <QTimer>

class ChatMsgItem;
class ChatMsgList : public QListWidget
{
  Q_OBJECT

public:
  explicit ChatMsgList(QWidget* parent = nullptr);

  void appendChatItem(ChatMsgItem* item);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void showEvent(QShowEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  QTimer* _debounce_timer;

signals:
  void sig_load_more_msg();
  void sig_clicked();
};

#endif  // CHATMSGLIST_HPP
