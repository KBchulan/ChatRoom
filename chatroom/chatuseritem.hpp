#ifndef CHATUSERITEM_HPP
#define CHATUSERITEM_HPP

#include <QWidget>
#include <QString>

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
