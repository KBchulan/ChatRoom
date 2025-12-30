#ifndef CHATDIALOG_HPP
#define CHATDIALOG_HPP

#include <QDialog>
#include <QAction>
#include <QStackedWidget>

namespace Ui
{
class ChatDialog;
}

class ChatUserList;
class ChatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ChatDialog(QWidget* parent = nullptr);
  ~ChatDialog();

// private slots:
//   void slot_load_chat_user();

private:
  void addTestData();

  Ui::ChatDialog* ui;
  QAction *_search_action;
  QAction* _clear_action;

  QStackedWidget* _stacked_widget;
  ChatUserList* _chat_user_list;
};

#endif  // CHATDIALOG_HPP
