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

void ContactItem::SetName(const QString& name)
{
  ui->name_label->setText(name);
}

void ContactItem::SetHead(const QString& head)
{
  QPixmap pixmap(head);
  ui->avatar_label->setPixmap(pixmap.scaled(ui->avatar_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ContactItem::SetHead(const QString& head, QSize size)
{
  QPixmap pixmap(head);
  ui->avatar_label->setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
