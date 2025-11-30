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

private:
  Ui::RegisterDialog* ui;
};

#endif  // REGISTERDIALOG_HPP
