/******************************************************************************
 *
 * @file       contactgroupitem.hpp
 * @brief      联系人列表的表头
 *
 * @author     KBchulan
 * @date       2026/01/14
 * @history
 ******************************************************************************/

#ifndef CONTACTGROUPITEM_HPP
#define CONTACTGROUPITEM_HPP

#include <QString>
#include <QWidget>

namespace Ui
{
class ContactGroupItem;
}

class ContactGroupItem : public QWidget
{
  Q_OBJECT

public:
  explicit ContactGroupItem(QWidget* parent = nullptr);
  ~ContactGroupItem();

  void SetLabelName(const QString& name);

private:
  Ui::ContactGroupItem* ui;
};

#endif  // CONTACTGROUPITEM_HPP
