/******************************************************************************
 *
 * @file       contactlist.hpp
 * @brief      联系人列表
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef CONTACTLIST_HPP
#define CONTACTLIST_HPP

#include <QMap>

#include "customlistwidget.hpp"
#include "global.hpp"

class ContactList : public CustomListWidget
{
  Q_OBJECT

public:
  explicit ContactList(QWidget* parent = nullptr);

  void AddContactItem(const QString& name, const QString& head);

private:
  void addNewFriendsItem();
  void ensureGroupExists(ContactGroup group);
  int getGroupEndRow(ContactGroup group);

  void addTestData();

  QMap<ContactGroup, QListWidgetItem*> _group_items;

signals:
  void sig_new_friends_clicked();
};

#endif  // CONTACTLIST_HPP
