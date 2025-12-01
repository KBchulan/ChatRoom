#include "registerdialog.hpp"
#include "global.hpp"

#include <QLineEdit>
#include <QRegularExpression>

#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog)
{
  ui->setupUi(this);

  // 设置组件模式
  ui->password_edit->setEchoMode(QLineEdit::Password);
  ui->confirm_edit->setEchoMode(QLineEdit::Password);

  // 设置 err_msg_label 属性
  ui->err_msg_label->setProperty("state", "normal");
  repolish(ui->err_msg_label);
}

RegisterDialog::~RegisterDialog()
{
  delete ui;
}

void RegisterDialog::on_verify_code_btn_clicked()
{
  auto email = ui->email_edit->text();
  QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

  // 格式不匹配，设置错误信息
  if (!regex.match(email).hasMatch()) {
    show_tip("邮箱格式不正确", false);
    return;
  }

  // 格式匹配则发送验证码
}

void RegisterDialog::show_tip(const QString &str, bool ok)
{
  if (ok) {
    ui->err_msg_label->setProperty("state", "normal");
  } else {
    ui->err_msg_label->setProperty("state", "error");
  }

  ui->err_msg_label->setText(str);
  repolish(ui->err_msg_label);
}

