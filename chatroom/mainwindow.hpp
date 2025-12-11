/******************************************************************************
 *
 * @file       mainwindow.hpp
 * @brief      主页面
 *
 * @author     KBchulan
 * @date       2025/11/28
 * @history
 *****************************************************************************/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QAction>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class LoginDialog;
class RegisterDialog;
class ResetPasswordDialog;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

public slots:
  void SlotSwitchRegister();
  void SlotSwitchLogin();
  void SlotSwitchResetPassword();

private:
  Ui::MainWindow* ui;
  QAction* _action;

  QStackedWidget* _stacked_widget;
  LoginDialog* _login_dialog;
  RegisterDialog* _register_dialog;
  ResetPasswordDialog* _reset_password_dialog;
};
#endif  // MAINWINDOW_HPP
