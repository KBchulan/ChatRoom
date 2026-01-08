/******************************************************************************
 *
 * @file       searchuseritem.hpp
 * @brief      搜索页面的单个 item 设计
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef SEARCHUSERITEM_HPP
#define SEARCHUSERITEM_HPP

#include <QWidget>

namespace Ui
{
class SearchUserItem;
}

class SearchUserItem : public QWidget
{
  Q_OBJECT

public:
  explicit SearchUserItem(QWidget* parent = nullptr);
  ~SearchUserItem();

private:
  Ui::SearchUserItem* ui;
};

#endif  // SEARCHUSERITEM_HPP
