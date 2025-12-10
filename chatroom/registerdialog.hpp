/******************************************************************************
 *
 * @file       registerdialog.hpp
 * @brief      注册页面
 *
 * @author     KBchulan
 * @date       2025/11/30
 * @history
 *****************************************************************************/

#ifndef REGISTERDIALOG_HPP
#define REGISTERDIALOG_HPP

#include <QTimer>
#include <QMap>
#include <QDialog>
#include <QString>
#include <QJsonObject>
#include <QRegularExpression>

#include "global.hpp"

namespace Ui
{
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
  Q_OBJECT

  static constexpr int kDefaultReturnCount = 5;

public:
  explicit RegisterDialog(QWidget* parent = nullptr);
  ~RegisterDialog();

  void Reset();

signals:
  void SigSwitchLogin();

private slots:
  void on_verify_code_btn_clicked();
  void on_confirm_btn_clicked();
  void on_return_button_clicked();
  void on_cancel_btn_clicked();
  void slot_reg_mod_finish(QString str, ErrorCode err, ReqID id);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;

  void show_tip(const QString& str, bool ok);
  void init_handlers();
  void check_user_valid();
  void check_email_valid();
  void check_password_valid();
  void check_confirm_password_valid();
  void check_verify_code_valid();

private:
  Ui::RegisterDialog* ui;
  QAction* toggle_password;
  QAction* toggle_confirm_password;
  QMap<ReqID, std::function<void(const QJsonObject&)>> _handlers;

  int _counter;
  QTimer* _return_timer;

  inline static const QRegularExpression email_regex{R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"};
  inline static const QRegularExpression password_regex{R"((?=.*[A-Za-z])(?=.*\d)[A-Za-z\d]{6,})"};
};

#endif  // REGISTERDIALOG_HPP
