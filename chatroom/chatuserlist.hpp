#ifndef CHATUSERLIST_HPP
#define CHATUSERLIST_HPP

#include <QEnterEvent>
#include <QEvent>
#include <QListWidget>
#include <QShowEvent>

class ChatUserList : public QListWidget
{
  Q_OBJECT
public:
  explicit ChatUserList(QWidget* parent = nullptr);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void showEvent(QShowEvent* event) override;

signals:
  void sig_load_chat_user();
};

#endif  // CHATUSERLIST_HPP
