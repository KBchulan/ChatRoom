#include "contactgroupitem.hpp"

#include "ui_contactgroupitem.h"

ContactGroupItem::ContactGroupItem(QWidget* parent) : QWidget(parent), ui(new Ui::ContactGroupItem)
{
  ui->setupUi(this);
  ui->group_name_label->setContentsMargins(10, 0, 0, 0);
}

ContactGroupItem::~ContactGroupItem()
{
  delete ui;
}

void ContactGroupItem::SetLabelName(const QString& name)
{
  ui->group_name_label->setText(name);
}
