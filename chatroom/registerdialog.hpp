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

#include <QDialog>
#include <QString>

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

private:
  void show_tip(const QString& str, bool ok);

private:
  Ui::RegisterDialog* ui;
};

#endif  // REGISTERDIALOG_HPP
