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

#include <QMap>
#include <QDialog>
#include <QString>
#include <QJsonObject>

#include "global.hpp"

namespace Ui
{
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
  Q_OBJECT

public:
  explicit RegisterDialog(QWidget* parent = nullptr);
  ~RegisterDialog();

private slots:
  void on_verify_code_btn_clicked();
  void on_confirm_btn_clicked();
  void slot_reg_mod_finish(QString str, ErrorCode err, ReqID id);

private:
  void show_tip(const QString& str, bool ok);
  void init_handlers();

private:
  Ui::RegisterDialog* ui;
  QMap<ReqID, std::function<void(const QJsonObject&)>> _handlers;
};

#endif  // REGISTERDIALOG_HPP
