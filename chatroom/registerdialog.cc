#include "registerdialog.hpp"

#include <QJsonDocument>
#include <QLineEdit>
#include <QToolButton>

#include "httpmanager.hpp"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::RegisterDialog), _counter(kDefaultReturnCount)
{
  ui->setupUi(this);

  // 初始页面
  ui->stackedWidget->setCurrentWidget(ui->page);

  // 注册处理回调
  init_handlers();

  // 设置组件模式
  ui->password_edit->setEchoMode(QLineEdit::Password);
  ui->confirm_edit->setEchoMode(QLineEdit::Password);

  // 设置 err_msg_label 属性
  ui->err_msg_label->clear();
  ui->err_msg_label->setProperty("state", "normal");
  repolish(ui->err_msg_label);

  // 安装过滤器，实现离开焦点即检测
  ui->user_edit->installEventFilter(this);
  ui->email_edit->installEventFilter(this);
  ui->password_edit->installEventFilter(this);
  ui->confirm_edit->installEventFilter(this);
  ui->verify_code_edit->installEventFilter(this);

  // 密码与确认密码的眼睛图标，可以切换密码样式
  _toggle_password = ui->password_edit->addAction(QIcon(":/icons/eye_close.png"), QLineEdit::TrailingPosition);

  _toggle_confirm_password = ui->confirm_edit->addAction(QIcon(":/icons/eye_close.png"), QLineEdit::TrailingPosition);

  // 设置眼睛图标按钮的鼠标悬停光标为小手
  for (auto* btn : ui->password_edit->findChildren<QToolButton*>())
  {
    btn->setCursor(Qt::PointingHandCursor);
  }
  for (auto* btn : ui->confirm_edit->findChildren<QToolButton*>())
  {
    btn->setCursor(Qt::PointingHandCursor);
  }

  // 设置返回定时器
  _return_timer = new QTimer(this);

  // 连接槽信息
  connect(&HttpManager::GetInstance(), &HttpManager::sig_register_mod_finish, this,
          &RegisterDialog::slot_reg_mod_finish);

  connect(_toggle_password, &QAction::triggered, this,
          [this]() -> void
          {
            if (ui->password_edit->echoMode() == QLineEdit::Password)
            {
              ui->password_edit->setEchoMode(QLineEdit::Normal);
              _toggle_password->setIcon(QIcon(":/icons/eye_open.png"));
            }
            else
            {
              ui->password_edit->setEchoMode(QLineEdit::Password);
              _toggle_password->setIcon(QIcon(":/icons/eye_close.png"));
            }
          });

  connect(_toggle_confirm_password, &QAction::triggered, this,
          [this]() -> void
          {
            if (ui->confirm_edit->echoMode() == QLineEdit::Password)
            {
              ui->confirm_edit->setEchoMode(QLineEdit::Normal);
              _toggle_confirm_password->setIcon(QIcon(":/icons/eye_open.png"));
            }
            else
            {
              ui->confirm_edit->setEchoMode(QLineEdit::Password);
              _toggle_confirm_password->setIcon(QIcon(":/icons/eye_close.png"));
            }
          });

  connect(_return_timer, &QTimer::timeout, this,
          [this]() -> void
          {
            --_counter;
            if (_counter <= 0)
            {
              emit SigSwitchLogin();
              return;
            }

            QString str = QString("注册成功, %1s 后返回登录").arg(_counter);
            ui->return_label->setText(str);
          });
}

RegisterDialog::~RegisterDialog()
{
  delete ui;
}

void RegisterDialog::Reset()
{
  // 清除输入框
  ui->err_msg_label->clear();
  ui->user_edit->clear();
  ui->email_edit->clear();
  ui->password_edit->clear();
  ui->confirm_edit->clear();
  ui->verify_code_edit->clear();

  // 重置密码显示状态
  ui->password_edit->setEchoMode(QLineEdit::Password);
  ui->confirm_edit->setEchoMode(QLineEdit::Password);
  _toggle_password->setIcon(QIcon(":/icons/eye_close.png"));
  _toggle_confirm_password->setIcon(QIcon(":/icons/eye_close.png"));

  // 重置定时器
  ui->verify_code_btn->Reset();
  _return_timer->stop();
  _counter = kDefaultReturnCount;

  // 重置返回提示文字
  ui->return_label->setText("注册成功, 5s 后返回登录");

  // 回到默认页面
  ui->stackedWidget->setCurrentWidget(ui->page);
}

void RegisterDialog::on_verify_code_btn_clicked()
{
  auto email = ui->email_edit->text();

  // 格式不匹配，设置错误信息
  if (!email_regex.match(email).hasMatch())
  {
    show_tip("邮箱格式不正确", false);
    return;
  }

  // 格式匹配则发送验证码
  QJsonObject dto;
  dto["email"] = email;
  dto["purpose"] = 1;

  HttpManager::GetInstance().PostHttpReq(QUrl(CHATROOM_API_BASE_URL + "/user/send-code"), dto,
                                         ReqID::ID_GET_VERIFY_CODE, Module::REGISTER);
}

void RegisterDialog::on_confirm_btn_clicked()
{
  auto name = ui->user_edit->text();
  if (name == "")
  {
    show_tip("昵称不能为空", false);
    return;
  }

  auto email = ui->email_edit->text();
  if (email == "")
  {
    show_tip("邮箱不能为空", false);
    return;
  }

  if (!email_regex.match(email).hasMatch())
  {
    show_tip("邮箱格式不正确", false);
    return;
  }

  auto password = ui->password_edit->text();
  if (password == "")
  {
    show_tip("密码不能为空", false);
    return;
  }

  auto confirm_pass = ui->confirm_edit->text();
  if (confirm_pass == "")
  {
    show_tip("确认密码不能为空", false);
    return;
  }

  if (password != confirm_pass)
  {
    show_tip("两次输入的密码不一致", false);
    return;
  }

  auto verify_code = ui->verify_code_edit->text();
  if (verify_code == "")
  {
    show_tip("验证码不能为空", false);
    return;
  }

  QJsonObject dto;
  dto["nickname"] = name;
  dto["email"] = email;
  dto["password"] = password;
  dto["confirm_password"] = confirm_pass;
  dto["verify_code"] = verify_code;
  dto["purpose"] = 1;

  HttpManager::GetInstance().PostHttpReq(QUrl(CHATROOM_API_BASE_URL + "/user/register"), dto, ReqID::ID_REGISTER,
                                         Module::REGISTER);
}

void RegisterDialog::on_return_button_clicked()
{
  emit SigSwitchLogin();
}

void RegisterDialog::on_cancel_btn_clicked()
{
  emit SigSwitchLogin();
}

void RegisterDialog::slot_reg_mod_finish(const QString& str, ErrorCode err, ReqID rid)
{
  if (err != ErrorCode::SUCCESS)
  {
    qDebug() << "当前请求存在网络错误\n";
  }

  // 如果没有错误
  QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
  if (jsonDoc.isNull())
  {
    show_tip("响应数据为空", false);
    return;
  }
  if (!jsonDoc.isObject())
  {
    show_tip("响应数据格式错误", false);
    return;
  }

  // 都正常解析就可以处理了
  _handlers[rid](jsonDoc.object());
}

bool RegisterDialog::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::FocusOut)
  {
    if (obj == ui->user_edit)
    {
      check_user_valid();
    }
    else if (obj == ui->email_edit)
    {
      check_email_valid();
    }
    else if (obj == ui->password_edit)
    {
      check_password_valid();
    }
    else if (obj == ui->confirm_edit)
    {
      check_confirm_password_valid();
    }
    else if (obj == ui->verify_code_edit)
    {
      check_verify_code_valid();
    }
  }
  return QDialog::eventFilter(obj, event);
}

void RegisterDialog::show_tip(const QString& str, bool is_ok)
{
  if (is_ok)
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
  _handlers.insert(ReqID::ID_GET_VERIFY_CODE,
                   [this](const QJsonObject& obj) -> void
                   {
                     auto code = obj["code"].toInt();
                     auto message = obj["message"].toString();

                     if (code != 0)
                     {
                       show_tip(message, false);
                       return;
                     }

                     show_tip("验证码已发送至邮箱，请于1分钟内完成注册", true);
                   });

  _handlers.insert(ReqID::ID_REGISTER,
                   [this](const QJsonObject& obj) -> void
                   {
                     auto code = obj["code"].toInt();
                     auto message = obj["message"].toString();

                     if (code != 0)
                     {
                       show_tip(message, false);
                       return;
                     }

                     // 提示注册成功并实现跳转，使用 stack page 进行操作
                     ui->stackedWidget->setCurrentWidget(ui->page_2);
                     _return_timer->start(1000);
                   });
}

void RegisterDialog::check_user_valid()
{
  if (ui->user_edit->text().isEmpty())
  {
    show_tip("用户名不能为空", false);
    return;
  }
  ui->err_msg_label->clear();
}

void RegisterDialog::check_email_valid()
{
  if (!email_regex.match(ui->email_edit->text()).hasMatch())
  {
    show_tip("邮箱格式不正确", false);
    return;
  }
  ui->err_msg_label->clear();
}

void RegisterDialog::check_password_valid()
{
  auto password = ui->password_edit->text();
  if (password.isEmpty())
  {
    show_tip("密码不能为空", false);
    return;
  }

  if (!password_regex.match(password).hasMatch())
  {
    show_tip("密码需不少于6位且含有数字和字母", false);
    return;
  }
  ui->err_msg_label->clear();
}

void RegisterDialog::check_confirm_password_valid()
{
  auto confirm_pass = ui->confirm_edit->text();
  if (confirm_pass.isEmpty())
  {
    show_tip("确认密码不能为空", false);
    return;
  }

  if (confirm_pass != ui->password_edit->text())
  {
    show_tip("两次密码输入不一致", false);
    return;
  }
  ui->err_msg_label->clear();
}

void RegisterDialog::check_verify_code_valid()
{
  if (ui->verify_code_edit->text().isEmpty())
  {
    show_tip("验证码不能为空", false);
    return;
  }
  ui->err_msg_label->clear();
}
