#include "mainwindow.hpp"
#include "logindialog.hpp"
#include "registerdialog.hpp"

#include <memory>

#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // 初始化其他窗口
  _login_dialog = std::make_unique<LoginDialog>();
  _register_dialog = std::make_unique<RegisterDialog>();

  // 连接信号槽
  connect(_login_dialog.get(), &LoginDialog::sig_switch_register, this, &MainWindow::SlotSwitchRegister);

  // 设置中心窗口
  setCentralWidget(_login_dialog.get());

  _login_dialog->show();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::SlotSwitchRegister()
{
  setCentralWidget(_register_dialog.get());
  _login_dialog->hide();
  _register_dialog->show();
}
