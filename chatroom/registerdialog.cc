#include "registerdialog.hpp"
#include "httpmanager.hpp"

#include <QLineEdit>
#include <QRegularExpression>
#include <QJsonDocument>

#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog)
{
  ui->setupUi(this);

  // 注册处理回调
  init_handlers();

  // 设置组件模式
  ui->password_edit->setEchoMode(QLineEdit::Password);
  ui->confirm_edit->setEchoMode(QLineEdit::Password);

  // 设置 err_msg_label 属性
  ui->err_msg_label->setProperty("state", "normal");
  repolish(ui->err_msg_label);

  // 连接槽信息
  connect(&HttpManager::GetInstance(), &HttpManager::sig_register_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
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
  if (!regex.match(email).hasMatch())
  {
    show_tip("邮箱格式不正确", false);
    return;
  }

  // 格式匹配则发送验证码
  QJsonObject dto;
  dto["email"] = email;
  dto["purpose"] = 1;

  HttpManager::GetInstance().PostHttpReq(QUrl(GateWayUrl + "/user/send-code"), dto, ReqID::ID_GET_VERIFY_CODE, Module::REGISTER);
}

void RegisterDialog::slot_reg_mod_finish(QString str, ErrorCode err, ReqID id)
{
  if (err != ErrorCode::SUCCESS)
  {
    show_tip("网络请求错误", false);
    return;
  }

  // 如果没有错误
  QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
  if (jsonDoc.isNull())
  {
    show_tip("Json 解析失败", false);
    return;
  }
  if (!jsonDoc.isObject())
  {
    show_tip("Json 解析错误", false);
    return;
  }

  // 都正常解析就可以处理了
  _handlers[id](jsonDoc.object());
}

void RegisterDialog::show_tip(const QString &str, bool ok)
{
  if (ok)
  {
    ui->err_msg_label->setProperty("state", "normal");
  }
  else
  {
    ui->err_msg_label->setProperty("state", "error");
  }

  ui->err_msg_label->setText(str);
  repolish(ui->err_msg_label);
}

void RegisterDialog::init_handlers()
{
  _handlers.insert(ReqID::ID_GET_VERIFY_CODE, [this](const QJsonObject& obj) -> void
  {
    auto code = obj["code"].toInt();
    auto message = obj["message"].toString();

    if (code != 0)
    {
      show_tip(message, false);
      return;
    }

    show_tip("验证码已发送至邮箱，请注意查收", true);
  });
}

