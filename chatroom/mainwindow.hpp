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
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class LoginDialog;
class RegisterDialog;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

public slots:
  void SlotSwitchRegister();

private:
  Ui::MainWindow* ui;
  std::unique_ptr<LoginDialog> _login_dialog;
  std::unique_ptr<RegisterDialog> _register_dialog;
};
#endif  // MAINWINDOW_HPP
