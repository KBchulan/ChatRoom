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

#include <QListWidget>

class ContactList : public QListWidget
{
  Q_OBJECT

public:
  explicit ContactList(QWidget* parent = nullptr);
};

#endif  // CONTACTLIST_HPP
