#include "mainwindow.hpp"

#include <QKeySequence>

#include "ui_mainwindow.h"
#include "chatdialog.hpp"
#include "global.hpp"
#include "logindialog.hpp"
#include "registerdialog.hpp"
#include "resetpassworddialog.hpp"
#include "tcpmanager.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // 初始化其他窗口
  _stacked_widget = new QStackedWidget(this);
  _login_dialog = new LoginDialog();
  _register_dialog = new RegisterDialog();
  _reset_password_dialog = new ResetPasswordDialog();
  _chat_dialog = new ChatDialog();

  _stacked_widget->addWidget(_login_dialog);
  _stacked_widget->addWidget(_register_dialog);
  _stacked_widget->addWidget(_reset_password_dialog);
  _stacked_widget->addWidget(_chat_dialog);

  // 设置中心窗口及大小
  setCentralWidget(_stacked_widget);
  _stacked_widget->setCurrentWidget(_login_dialog);
  resize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);

  // 连接信号槽
  connect(_login_dialog, &LoginDialog::SigSwitchRegister, this, &MainWindow::SlotSwitchRegister);
  connect(_register_dialog, &RegisterDialog::SigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
  connect(_login_dialog, &LoginDialog::SigForgetPassword, this, &MainWindow::SlotSwitchResetPassword);
  connect(_reset_password_dialog, &ResetPasswordDialog::SigSwitchToLogin, this, &MainWindow::SlotSwitchLogin);
  connect(&TcpManager::GetInstance(), &TcpManager::sig_switch_chat_dialog, this, &MainWindow::SlotSwitchChat);

  // 绑定快捷键
  _action = new QAction(this);

  // ctrl + q 退出
  _action->setShortcut(QKeySequence::Quit);
  connect(_action, &QAction::triggered, this,
          [this]()
          {
            TcpManager::GetInstance().Disconnect();
            close();
          });

  addAction(_action);

  // FIXME: 这里是模拟，聊天页面开发完成需要删除
  // emit TcpManager::GetInstance().sig_switch_chat_dialog();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::SlotSwitchRegister()
{
  _stacked_widget->setCurrentWidget(_register_dialog);
  _login_dialog->Reset();
}

void MainWindow::SlotSwitchLogin()
{
  _stacked_widget->setCurrentWidget(_login_dialog);
  _register_dialog->Reset();
  _reset_password_dialog->Reset();
}

void MainWindow::SlotSwitchResetPassword()
{
  _stacked_widget->setCurrentWidget(_reset_password_dialog);
  _login_dialog->Reset();
}

void MainWindow::SlotSwitchChat()
{
  resize(UIConstants::WindowChatWidth, UIConstants::WindowChatHeight);
  _stacked_widget->setCurrentWidget(_chat_dialog);
  _login_dialog->Reset();
}
