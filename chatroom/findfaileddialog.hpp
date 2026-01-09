/******************************************************************************
 *
 * @file       findfaileddialog.hpp
 * @brief      搜索失败的用户
 *
 * @author     KBchulan
 * @date       2026/01/09
 * @history
 ******************************************************************************/

#ifndef FINDFAILEDDIALOG_HPP
#define FINDFAILEDDIALOG_HPP

#include <QDialog>

namespace Ui
{
class FindFailedDialog;
}

class FindFailedDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FindFailedDialog(QWidget* parent = nullptr);
  ~FindFailedDialog();

private:
  Ui::FindFailedDialog* ui;

private slots:
  void on_confirm_button_clicked();
};

#endif  // FINDFAILEDDIALOG_HPP
