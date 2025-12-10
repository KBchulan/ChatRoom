#include "logindialog.hpp"

#include <QPushButton>

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog)
{
  ui->setupUi(this);

  // 连接信号
  connect(ui->register_button, &QPushButton::clicked, this, &LoginDialog::SigSwitchRegister);
}

LoginDialog::~LoginDialog()
{
  delete ui;
}
