/******************************************************************************
 *
 * @file       findsuccessdialog.hpp
 * @brief      搜索成功的用户
 *
 * @author     KBchulan
 * @date       2026/01/09
 * @history
 ******************************************************************************/

#ifndef FINDSUCCESSDIALOG_HPP
#define FINDSUCCESSDIALOG_HPP

#include <QDialog>

namespace Ui
{
class FindSuccessDialog;
}

class FindSuccessDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FindSuccessDialog(QWidget* parent = nullptr);
  ~FindSuccessDialog();

  void SetName(const QString& name);

private:
  Ui::FindSuccessDialog* ui;

private slots:
  void on_confirm_button_clicked();
};

#endif  // FINDSUCCESSDIALOG_HPP
