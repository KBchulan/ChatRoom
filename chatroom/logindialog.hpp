/******************************************************************************
 *
 * @file       logindialog.hpp
 * @brief      登录页面
 *
 * @author     KBchulan
 * @date       2025/11/28
 * @history
 *****************************************************************************/


#ifndef LOGINDIALOG_HPP
#define LOGINDIALOG_HPP

#include <QDialog>
#include <QEvent>
#include <QMap>
#include <QRegularExpression>

#include "global.hpp"
#include "serverinfo.hpp"

namespace Ui
{
class LoginDialog;
}

class LoginDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LoginDialog(QWidget* parent = nullptr);
  ~LoginDialog();

  void Reset();

private:
  void show_tip(const QString& str, bool ok);
  void init_handlers();
  void init_header();
  void check_user_valid();
  void check_password_valid();

private:
  Ui::LoginDialog* ui;
  QAction* _toggle_password;
  QMap<ReqID, std::function<void(const QJsonObject&)>> _handlers;

  inline static const QRegularExpression email_regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"};
  inline static const QRegularExpression password_regex{R"((?=.*[A-Za-z])(?=.*\d)[A-Za-z\d]{6,})"};

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

signals:
  void SigSwitchRegister();
  void SigForgetPassword();
  void SigConnectTcp(ServerInfo);

private slots:
  void on_login_button_clicked();
  void slot_login_mod_finish(QString str, ErrorCode err, ReqID id);
};

#endif  // LOGINDIALOG_HPP
