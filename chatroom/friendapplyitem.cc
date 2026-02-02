#include "friendapplyitem.hpp"

#include <QPixmap>
#include <QPushButton>

#include "ui_friendapplyitem.h"

FriendApplyItem::FriendApplyItem(QWidget* parent) : QWidget(parent), ui(new Ui::FriendApplyItem)
{
  ui->setupUi(this);

  // 默认显示同意按钮
  ui->action_stack->setCurrentWidget(ui->accept_page);

  connect(ui->accept_button, &QPushButton::clicked, this, &FriendApplyItem::slot_accept_clicked);
}

FriendApplyItem::~FriendApplyItem()
{
  delete ui;
}

void FriendApplyItem::SetHead(const QString& head)
{
  _head = head;
  QPixmap pixmap(head);
  ui->avatar_label->setPixmap(pixmap.scaled(ui->avatar_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void FriendApplyItem::SetName(const QString& name)
{
  _name = name;
  ui->name_label->setText(name);
}

void FriendApplyItem::SetMsg(const QString& msg)
{
  _msg = msg;
  ui->msg_label->setText(msg);
}

void FriendApplyItem::SetStatus(ApplyStatus status)
{
  _status = status;
  if (status == ApplyStatus::Accepted)
  {
    ui->action_stack->setCurrentWidget(ui->added_page);
  }
  else
  {
    ui->action_stack->setCurrentWidget(ui->accept_page);
  }
}

void FriendApplyItem::SetTimestamp(std::int64_t timestamp)
{
  _timestamp = timestamp;
}

void FriendApplyItem::slot_accept_clicked()
{
  emit sig_accept_clicked(this);
}
