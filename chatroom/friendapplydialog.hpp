/******************************************************************************
 *
 * @file       friendapplydialog.hpp
 * @brief      好友申请对话框
 *
 * @author     KBchulan
 * @date       2026/01/10
 * @history
 *****************************************************************************/

#ifndef FRIENDAPPLYDIALOG_HPP
#define FRIENDAPPLYDIALOG_HPP

#include <QDialog>

namespace Ui
{
class FriendApplyDialog;
}

class FriendApplyDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FriendApplyDialog(QWidget* parent = nullptr);
  ~FriendApplyDialog() override;

  void SetUserInfo(const QString& head, const QString& name);

private:
  Ui::FriendApplyDialog* ui;

private slots:
  void on_cancel_button_clicked();
  void on_send_button_clicked();
};

#endif  // FRIENDAPPLYDIALOG_HPP
