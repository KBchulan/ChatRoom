#include "findfaileddialog.hpp"

#include <QShortcut>

#include "ui_findfaileddialog.h"

FindFailedDialog::FindFailedDialog(QWidget* parent) : QDialog(parent), ui(new Ui::FindFailedDialog)
{
  ui->setupUi(this);

  // Ctrl+Q 关闭对话框
  auto* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
  connect(shortcut, &QShortcut::activated, this, &QDialog::close);

  // 设置窗口标题，并应用无边框对话框样式
  this->setWindowTitle("失败");
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

  // 设置 ui 样式
  ui->tip_label->setText("该用户不存在，请检查输入是否正确");
  ui->confirm_button->setCursor(Qt::PointingHandCursor);
}

FindFailedDialog::~FindFailedDialog()
{
  delete ui;
}

void FindFailedDialog::on_confirm_button_clicked()
{
  this->close();
}
