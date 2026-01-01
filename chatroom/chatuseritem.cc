#include "chatuseritem.hpp"

#include <QPixmap>

#include "ui_chatuseritem.h"

ChatUserItem::ChatUserItem(QWidget* parent) : QWidget(parent), ui(new Ui::ChatUserItem)
{
  ui->setupUi(this);
}

ChatUserItem::~ChatUserItem()
{
  delete ui;
}

void ChatUserItem::SetHead(const QString& head)
{
  QPixmap pixmap(head);

  ui->avatar_label->setPixmap(pixmap.scaled(ui->avatar_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  ui->avatar_label->setScaledContents(true);
}

void ChatUserItem::SetName(const QString& name)
{
  ui->user_name_label->setText(name);
}

void ChatUserItem::SetMsg(const QString& msg)
{
  ui->user_chat_msg_label->setText(msg);
}

void ChatUserItem::SetTime(const QString& time)
{
  ui->time_label->setText(time);
}

void ChatUserItem::UpdateItem(const QString& head, const QString& name, const QString& msg, const QString& time)
{
  SetHead(head);
  SetName(name);
  SetMsg(msg);
  SetTime(time);
}
