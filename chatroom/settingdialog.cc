#include "settingdialog.hpp"

#include <QShortcut>

#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingDialog)
{
  ui->setupUi(this);

  // Ctrl+Q 关闭对话框
  auto* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
  connect(shortcut, &QShortcut::activated, this, &QDialog::close);
}

SettingDialog::~SettingDialog()
{
  delete ui;
}
