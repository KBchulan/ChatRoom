#include "mainwindow.hpp"

#include <QKeySequence>

#include "chatdialog.hpp"
#include "global.hpp"
#include "logindialog.hpp"
#include "registerdialog.hpp"
#include "resetpassworddialog.hpp"
#include "tcpmanager.hpp"
#include "ui_mainwindow.h"

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
  setMinimumSize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  resize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  _stacked_widget->setCurrentWidget(_login_dialog);

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
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::SlotSwitchRegister()
{
  setMinimumSize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  resize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  _stacked_widget->setCurrentWidget(_register_dialog);
  _login_dialog->Reset();
}

void MainWindow::SlotSwitchLogin()
{
  setMinimumSize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  resize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  _stacked_widget->setCurrentWidget(_login_dialog);
  _register_dialog->Reset();
  _reset_password_dialog->Reset();
}

void MainWindow::SlotSwitchResetPassword()
{
  setMinimumSize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  resize(UIConstants::WindowLoginWidth, UIConstants::WindowLoginHeight);
  _stacked_widget->setCurrentWidget(_reset_password_dialog);
  _login_dialog->Reset();
}

void MainWindow::SlotSwitchChat()
{
  setMinimumSize(UIConstants::WindowChatWidth, UIConstants::WindowChatHeight);
  resize(UIConstants::WindowChatWidth, UIConstants::WindowChatHeight);
  _stacked_widget->setCurrentWidget(_chat_dialog);
  _login_dialog->Reset();
}
