#include "findsuccessdialog.hpp"

#include <QPixmap>
#include <QShortcut>

#include "friendapplydialog.hpp"
#include "ui_findsuccessdialog.h"

FindSuccessDialog::FindSuccessDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FindSuccessDialog)
{
  ui->setupUi(this);

  // Ctrl+Q 关闭对话框
  auto* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
  connect(shortcut, &QShortcut::activated, this, &QDialog::close);

  // 设置窗口标题，并应用无边框对话框样式
  this->setWindowTitle("添加");
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

  // 设置按钮为 小手 图标
  ui->confirm_button->setCursor(Qt::PointingHandCursor);
}

FindSuccessDialog::~FindSuccessDialog()
{
  delete ui;
}

void FindSuccessDialog::SetUserInfo(const QString& head, const QString& name)
{
  _head = head;
  _name = name;

  QPixmap pixmap(head);
  pixmap = pixmap.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->head_label->setPixmap(pixmap);
  ui->name_label->setText(name);
}

void FindSuccessDialog::on_confirm_button_clicked()
{
  auto* dialog = new FriendApplyDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->SetUserInfo(_head, _name);
  connect(dialog, &QDialog::finished, this, &QDialog::close);
  dialog->exec();
}
