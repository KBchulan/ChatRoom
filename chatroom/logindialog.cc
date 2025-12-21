#include "logindialog.hpp"
#include "httpmanager.hpp"
#include "tcpmanager.hpp"
#include "userinfo.hpp"

#include <QAction>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QToolButton>
#include <QJsonObject>
#include <QJsonDocument>

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog), _server_info("", 0, "")
{
  ui->setupUi(this);

  // 初始化回调
  init_handlers();

  // 初始化头像
  init_header();

  // 设置 ui 元素
  ui->err_msg_label->clear();
  ui->user_edit->setPlaceholderText("请输入邮箱或用户名");
  ui->password_edit->setPlaceholderText("请输入密码");
  ui->password_edit->setEchoMode(QLineEdit::Password);

  // 注册过滤事件
  ui->user_edit->installEventFilter(this);
  ui->password_edit->installEventFilter(this);

  // 密码的眼睛图标，可以切换密码样式
  _toggle_password = ui->password_edit->addAction(
    QIcon(":/icons/eye_close.png"),
    QLineEdit::TrailingPosition
  );

  // 设置眼睛图标按钮的鼠标悬停光标为小手
  for (auto* btn : ui->password_edit->findChildren<QToolButton*>())
    btn->setCursor(Qt::PointingHandCursor);

  // 忘记密码 - 设置悬浮效果和点击
  ui->forget_password_label->setCursor(Qt::PointingHandCursor);
  ui->forget_password_label->installEventFilter(this);

  // 连接信号
  connect(ui->register_button, &QPushButton::clicked, this, &LoginDialog::SigSwitchRegister);
  connect(&HttpManager::GetInstance(), &HttpManager::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
  connect(this, &LoginDialog::SigConnectTcp, &TcpManager::GetInstance(), &TcpManager::SlotTcpConnect);
  connect(&TcpManager::GetInstance(), &TcpManager::sig_conn_finish, this, &LoginDialog::slot_tcp_conn_finish);
  connect(&TcpManager::GetInstance(), &TcpManager::sig_login_failed, this, &LoginDialog::slot_login_failed);

  connect(_toggle_password, &QAction::triggered, this, [this]() -> void
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
}

LoginDialog::~LoginDialog()
{
  delete ui;
}

void LoginDialog::Reset()
{
  // 清除输入框
  ui->err_msg_label->clear();
  ui->user_edit->clear();
  ui->password_edit->clear();

  // 重置密码显示状态
  ui->password_edit->setEchoMode(QLineEdit::Password);
  _toggle_password->setIcon(QIcon(":/icons/eye_close.png"));
}

void LoginDialog::show_tip(const QString &str, bool ok)
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

void LoginDialog::init_handlers()
{
  _handlers.insert(ReqID::ID_LOGIN, [this](const QJsonObject& obj) -> void
  {
    // 根据定义结构解析
    auto code = obj["code"].toInt();
    auto message = obj["message"].toString();
    auto uuid = obj["data"]["uuid"].toString();
    auto host = obj["data"]["host"].toString();
    auto port = obj["data"]["port"].toInt();
    auto token = obj["data"]["token"].toString();
    auto jwt_token = obj["data"]["jwt_token"].toString();

    if (code != 0)
    {
      show_tip(message, false);
      return;
    }

    ServerInfo server_info(host, static_cast<uint16_t>(port), token);
    _server_info = std::move(server_info);

    UserInfo::GetInstance().SetUUID(uuid);
    UserInfo::GetInstance().SetJwtToken(jwt_token);

    show_tip("登录成功，正在连接服务器", true);
    emit SigConnectTcp(server_info);
  });
}

void LoginDialog::init_header()
{
  // 加载原始头像
  QPixmap origin_pixmap(":/avatar/avatar.ico");
  if (origin_pixmap.isNull())
  {
    return;
  }

  // 缩放到目标大小
  constexpr int size = 240;
  constexpr int border_width = 3;
  QPixmap scaled = origin_pixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // 创建圆形头像
  QPixmap rounded(size, size);
  rounded.fill(Qt::transparent);

  QPainter painter(&rounded);
  painter.setRenderHint(QPainter::Antialiasing);

  // 画圆形头像
  QPainterPath path;
  path.addEllipse(0, 0, size, size);
  painter.setClipPath(path);
  painter.drawPixmap(0, 0, scaled);

  // 画边框
  painter.setClipping(false);
  QPen pen(QColor(232, 228, 220), border_width);
  painter.setPen(pen);
  painter.drawEllipse(border_width / 2, border_width / 2, size - border_width, size - border_width);

  ui->avatar_label->setPixmap(rounded);
}

void LoginDialog::check_user_valid()
{
  auto user = ui->user_edit->text();
  if (user.isEmpty())
  {
    show_tip("用户名不能为空", false);
    return;
  }

  // 有 @ 证明是邮箱登录
  if (user.contains('@') && !email_regex.match(user).hasMatch())
  {
    show_tip("邮箱格式不合规范", false);
    return;
  }

  ui->err_msg_label->clear();
}

void LoginDialog::check_password_valid()
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

bool LoginDialog::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == ui->forget_password_label)
  {
    if (event->type() == QEvent::Enter)
    {
      ui->forget_password_label->setStyleSheet("color: #5DADE2;");
    }
    else if (event->type() == QEvent::Leave)
    {
      ui->forget_password_label->setStyleSheet("");
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
      emit SigForgetPassword();
    }
  }

  if (event->type() == QEvent::FocusOut)
  {
    if (watched == ui->user_edit) check_user_valid();
    else if (watched == ui->password_edit) check_password_valid();
  }

  return QDialog::eventFilter(watched, event);
}

void LoginDialog::on_login_button_clicked()
{
  auto user = ui->user_edit->text();
  if (user.isEmpty())
  {
    show_tip("用户名不能为空", false);
    return;
  }

  // 是邮箱登录
  if (user.contains('@') && !email_regex.match(user).hasMatch())
  {
    show_tip("邮箱格式不合规范", false);
    return;
  }

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

  QJsonObject dto;
  dto["user"] = user;
  dto["password"] = password;

  HttpManager::GetInstance().PostHttpReq(QUrl(CHATROOM_API_BASE_URL + "/user/login"), dto, ReqID::ID_LOGIN, Module::LOGIN);
}

void LoginDialog::slot_login_mod_finish(QString str, ErrorCode err, ReqID id)
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
  _handlers[id](jsonDoc.object());
}

void LoginDialog::slot_tcp_conn_finish(bool success)
{
  if (success)
  {
    show_tip("连接服务器成功，正在校验信息", true);

    QJsonObject obj;
    obj["uuid"] = UserInfo::GetInstance().GetUUID();
    obj["token"] = _server_info.GetToken();

    QJsonDocument doc{obj};
    QString jsonStr = doc.toJson(QJsonDocument::Indented);

    TcpManager::GetInstance().sig_send_data(ReqID::ID_LOGIN_CHAT, jsonStr);
  }
  else
  {
    show_tip("网络异常，连接服务器失败", false);
  }
}

void LoginDialog::slot_login_failed(int error_code)
{
  QString result = QString("登录失败，错误码为: %1").arg(error_code);
  show_tip(result, false);
}
