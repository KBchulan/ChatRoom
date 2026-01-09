#include "findsuccessdialog.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QPixmap>
#include <QShortcut>
#include <QString>

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

  // 检查 static 目录是否存在，不存在则创建
  QString app_dir = QCoreApplication::applicationDirPath();
  QString static_dir = app_dir + QDir::separator() + "static";

  if (!QDir(static_dir).exists())
  {
    QDir().mkdir(static_dir);
  }

  QString pix_path = static_dir + QDir::separator() + "test1.ico";

  QPixmap pixmap(pix_path);
  pixmap = pixmap.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->head_label->setPixmap(pixmap);
}

FindSuccessDialog::~FindSuccessDialog()
{
  delete ui;
}

void FindSuccessDialog::SetName(const QString& name)
{
  ui->name_label->setText(name);
}

void FindSuccessDialog::on_confirm_button_clicked()
{
  // TODO: 添加好友逻辑
}
