/******************************************************************************
 *
 * @file       chatuserlist.hpp
 * @brief      自定义 QListWidget 用于显示聊天用户列表
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATUSERLIST_HPP
#define CHATUSERLIST_HPP

#include <QShowEvent>
#include <QTimer>

#include "customlistwidget.hpp"

class ChatUserList : public CustomListWidget
{
  Q_OBJECT
public:
  explicit ChatUserList(QWidget* parent = nullptr);

  void addUserItem(const QString& head, const QString& name, const QString& msg, const QString& time);

private slots:
  void slot_load_more();

private:
  void showLoading();
  void hideLoading();
  void addTestData();

  QTimer* _debounce_timer;
  bool _loading = false;
  QListWidgetItem* _loading_item = nullptr;
};

#endif  // CHATUSERLIST_HPP
