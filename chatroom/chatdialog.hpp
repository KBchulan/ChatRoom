/******************************************************************************
 *
 * @file       chatdialog.hpp
 * @brief      主聊天页面
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATDIALOG_HPP
#define CHATDIALOG_HPP

#include <QAction>
#include <QDialog>
#include <QListWidgetItem>
#include <QStackedWidget>

namespace Ui
{
class ChatDialog;
}

class ChatPage;
class ChatUserList;
class ChatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ChatDialog(QWidget* parent = nullptr);
  ~ChatDialog();

private:
  void addTestData();
  void showLoading();
  void hideLoading();

  Ui::ChatDialog* ui;
  QAction* _search_action;
  QAction* _clear_action;

  QStackedWidget* _mid_stacked_widget;
  ChatUserList* _chat_user_list;

  QStackedWidget* _right_stacked_widget;
  ChatPage* _chat_page;

  bool _loading;
  QListWidgetItem* _loading_item;

private slots:
  void slot_load_chat_user();
};

#endif  // CHATDIALOG_HPP
