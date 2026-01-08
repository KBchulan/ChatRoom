#include "contactitem.hpp"

#include "ui_contactitem.h"

ContactItem::ContactItem(QWidget* parent) : QWidget(parent), ui(new Ui::ContactItem)
{
  ui->setupUi(this);
}

ContactItem::~ContactItem()
{
  delete ui;
}
