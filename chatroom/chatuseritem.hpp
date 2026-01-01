/******************************************************************************
 *
 * @file       chatuseritem.hpp
 * @brief      单个聊天用户项，用于设计 ui
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATUSERITEM_HPP
#define CHATUSERITEM_HPP

#include <QString>
#include <QWidget>

namespace Ui
{
class ChatUserItem;
}

class ChatUserItem : public QWidget
{
  Q_OBJECT

public:
  explicit ChatUserItem(QWidget* parent = nullptr);
  ~ChatUserItem();

  void SetHead(const QString& head);
  void SetName(const QString& name);
  void SetMsg(const QString& msg);
  void SetTime(const QString& time);
  void UpdateItem(const QString& head, const QString& name, const QString& msg, const QString& time);

private:
  Ui::ChatUserItem* ui;
};

#endif  // CHATUSERITEM_HPP
