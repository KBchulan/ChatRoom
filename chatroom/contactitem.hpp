/******************************************************************************
 *
 * @file       contactitem.hpp
 * @brief      单个联系人的具体设计
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef CONTACTITEM_HPP
#define CONTACTITEM_HPP

#include <QWidget>

namespace Ui
{
class ContactItem;
}

class ContactItem : public QWidget
{
  Q_OBJECT

public:
  explicit ContactItem(QWidget* parent = nullptr);
  ~ContactItem();

private:
  Ui::ContactItem* ui;
};

#endif  // CONTACTITEM_HPP
