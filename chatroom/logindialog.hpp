/******************************************************************************
 *
 * @file       logindialog.hpp
 * @brief      登录页面
 *
 * @author     KBchulan
 * @date       2025/11/28
 * @history
 *****************************************************************************/


#ifndef LOGINDIALOG_HPP
#define LOGINDIALOG_HPP

#include <QDialog>
#include <QEvent>

namespace Ui
{
class LoginDialog;
}

class LoginDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LoginDialog(QWidget* parent = nullptr);
  ~LoginDialog();

private:
  Ui::LoginDialog* ui;

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

signals:
  void SigSwitchRegister();
  void SigForgetPassword();
};

#endif  // LOGINDIALOG_HPP
