#include "logindialog.hpp"

#include <QLabel>
#include <QPushButton>

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog)
{
  ui->setupUi(this);

  // 忘记密码 - 设置悬浮效果和点击
  ui->forget_password_label->setCursor(Qt::PointingHandCursor);
  ui->forget_password_label->installEventFilter(this);

  // 连接信号
  connect(ui->register_button, &QPushButton::clicked, this, &LoginDialog::SigSwitchRegister);
}

LoginDialog::~LoginDialog()
{
  delete ui;
}

bool LoginDialog::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == ui->forget_password_label) {
    if (event->type() == QEvent::Enter) {
      ui->forget_password_label->setStyleSheet("color: #5DADE2;");
    } else if (event->type() == QEvent::Leave) {
      ui->forget_password_label->setStyleSheet("");
    } else if (event->type() == QEvent::MouseButtonRelease) {
      emit SigForgetPassword();
    }
  }
  return QDialog::eventFilter(watched, event);
}
