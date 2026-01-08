/******************************************************************************
 *
 * @file       searchuserlist.hpp
 * @brief      搜索结果的list展示
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef SEARCHUSERLIST_HPP
#define SEARCHUSERLIST_HPP

#include <QListWidget>

class SearchUserList : public QListWidget
{
  Q_OBJECT

public:
  explicit SearchUserList(QWidget* parent = nullptr);
};

#endif  // SEARCHUSERLIST_HPP
