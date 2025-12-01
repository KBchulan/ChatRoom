#include "mainwindow.hpp"
#include "logindialog.hpp"
#include "registerdialog.hpp"

#include <memory>
#include <QKeySequence>

#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // 初始化其他窗口
  _login_dialog = std::make_unique<LoginDialog>();
  _register_dialog = std::make_unique<RegisterDialog>();

  // 连接信号槽
  connect(_login_dialog.get(), &LoginDialog::sig_switch_register, this, &MainWindow::SlotSwitchRegister);

  // 绑定快捷键
  action = std::make_unique<QAction>();

  // ctrl + q 退出
  action->setShortcut(QKeySequence::Quit);
  connect(action.get(), &QAction::triggered, this, &QWidget::close);

  addAction(action.get());

  // 设置中心窗口
  setCentralWidget(_login_dialog.get());
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::SlotSwitchRegister()
{
  setCentralWidget(_register_dialog.get());
}
