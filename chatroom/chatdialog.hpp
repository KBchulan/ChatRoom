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
#include <QPointer>
#include <QStackedWidget>

#include "sidebarwidget.hpp"

namespace Ui
{
class ChatDialog;
}

class ChatPage;
class FriendApplyPage;
class ChatUserList;
class ContactList;
class SearchUserList;
class SettingDialog;
class ChatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ChatDialog(QWidget* parent = nullptr);
  ~ChatDialog();

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  void handleTextChange(const QString& text);

  Ui::ChatDialog* ui;
  QAction* _search_action;
  QAction* _clear_action;

  QPointer<QWidget> _prev_mid_widget;

  QStackedWidget* _mid_stacked_widget;
  ChatUserList* _chat_user_list;
  ContactList* _contact_list;
  SearchUserList* _search_user_list;

  QStackedWidget* _right_stacked_widget;
  ChatPage* _chat_page;
  QWidget* _contact_page;

  // _contact_page 的子控件
  FriendApplyPage* _friend_apply_page;

  SettingDialog* _setting_dialog;

  QString _static_dir;

private slots:
  void slot_item_changed(SideBarItemType type);
  void slot_search_item_clicked(const QString& uuid);
  void slot_search_failed();
  void on_add_button_clicked();
  void slot_new_friends_clicked();
};

#endif  // CHATDIALOG_HPP
