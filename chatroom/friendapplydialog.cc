#include "friendapplydialog.hpp"

#include <QPixmap>
#include <QShortcut>

#include "ui_friendapplydialog.h"

FriendApplyDialog::FriendApplyDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FriendApplyDialog)
{
  ui->setupUi(this);

  // Ctrl+Q 关闭对话框
  auto* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
  connect(shortcut, &QShortcut::activated, this, &QDialog::close);

  // 设置窗口标题，并应用无边框对话框样式
  this->setWindowTitle("添加好友");
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

  // 设置按钮为 小手 图标
  ui->cancel_button->setCursor(Qt::PointingHandCursor);
  ui->send_button->setCursor(Qt::PointingHandCursor);
}

FriendApplyDialog::~FriendApplyDialog()
{
  delete ui;
}

void FriendApplyDialog::SetUserInfo(const QString& head, const QString& name)
{
  QPixmap pixmap(head);
  pixmap = pixmap.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->head_label->setPixmap(pixmap);
  ui->name_label->setText(name);
}

void FriendApplyDialog::on_cancel_button_clicked()
{
  this->reject();
}

void FriendApplyDialog::on_send_button_clicked()
{
  QString remark_msg = ui->remark_edit->text().trimmed();
  QString apply_msg = ui->apply_edit->text().trimmed();
  // TODO: 发送好友申请逻辑
  this->accept();
}
