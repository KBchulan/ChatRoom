#include "searchuseritem.hpp"

#include <QPixmap>

#include "ui_searchuseritem.h"

SearchUserItem::SearchUserItem(QWidget* parent) : QWidget(parent), ui(new Ui::SearchUserItem)
{
  ui->setupUi(this);
}

SearchUserItem::~SearchUserItem()
{
  delete ui;
}

void SearchUserItem::SetHead(const QString& head)
{
  QPixmap pixmap(head);
  ui->avatar_label->setPixmap(pixmap.scaled(ui->avatar_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  ui->avatar_label->setScaledContents(true);
}

void SearchUserItem::SetName(const QString& name)
{
  ui->name_label->setText(name);
}

void SearchUserItem::SetDesc(const QString& desc)
{
  ui->desc_label->setText(desc);
}

void SearchUserItem::SetUuid(const QString& uuid)
{
  _uuid = uuid;
}

void SearchUserItem::UpdateItem(const QString& uuid, const QString& head, const QString& name, const QString& desc)
{
  SetUuid(uuid);
  SetHead(head);
  SetName(name);
  SetDesc(desc);
}

QString SearchUserItem::GetUuid() const
{
  return _uuid;
}
