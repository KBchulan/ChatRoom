/******************************************************************************
 *
 * @file       resetpassworddialog.hpp
 * @brief      重置密码的页面
 *
 * @author     KBchulan
 * @date       2025/12/11
 * @history
 *****************************************************************************/

#ifndef RESETPASSWORDDIALOG_HPP
#define RESETPASSWORDDIALOG_HPP

#include <QAction>
#include <QDialog>
#include <QRegularExpression>

#include "global.hpp"

namespace Ui
{
class ResetPasswordDialog;
}

class ResetPasswordDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ResetPasswordDialog(QWidget* parent = nullptr);
  ~ResetPasswordDialog();

  void Reset();

protected:
  bool eventFilter(QObject* obj, QEvent* event) override;

signals:
  void SigSwitchToLogin();

private slots:
  void on_return_btn_clicked();
  void on_verify_code_btn_clicked();
  void on_confirm_btn_clicked();
  void slot_reset_mod_finish(const QString& str, ErrorCode err, ReqID rid);

private:
  void show_tip(const QString& str, bool is_ok);
  void init_handler();
  void check_email_valid();
  void check_password_valid();
  void check_confirm_password_valid();
  void check_verify_code_valid();

private:
  Ui::ResetPasswordDialog* ui;
  QAction* _toggle_password;
  QAction* _toggle_confirm_password;
  QMap<ReqID, std::function<void(const QJsonObject&)>> _handlers;

  inline static const QRegularExpression email_regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"};
  inline static const QRegularExpression password_regex{R"((?=.*[A-Za-z])(?=.*\d)[A-Za-z\d]{6,})"};
};

#endif  // RESETPASSWORDDIALOG_HPP
