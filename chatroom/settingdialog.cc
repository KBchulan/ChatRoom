#include "settingdialog.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QShortcut>

#include "global.hpp"
#include "tcpmanager.hpp"
#include "ui_settingdialog.h"
#include "userinfo.hpp"

SettingDialog::SettingDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingDialog)
{
  ui->setupUi(this);

  // Ctrl+Q 关闭对话框
  auto* shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
  connect(shortcut, &QShortcut::activated, this, &QDialog::close);
}

SettingDialog::~SettingDialog()
{
  delete ui;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void SettingDialog::on_exit_button_clicked()
{
  // 发送退出登录请求
  QJsonObject obj;
  obj["uuid"] = UserInfo::GetInstance().GetUUID();

  QJsonDocument doc{obj};
  QString jsonStr = doc.toJson(QJsonDocument::Compact);

  TcpManager::GetInstance().sig_send_data(ReqID::ID_EXIT_LOGIN, jsonStr);
}
