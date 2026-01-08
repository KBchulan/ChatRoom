/******************************************************************************
 *
 * @file       settingdialog.hpp
 * @brief      设置页面，支持 ctrl + Q 快捷键关闭
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef SETTINGDIALOG_HPP
#define SETTINGDIALOG_HPP

#include <QDialog>

namespace Ui
{
class SettingDialog;
}

class SettingDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingDialog(QWidget* parent = nullptr);
  ~SettingDialog();

private:
  Ui::SettingDialog* ui;
};

#endif  // SETTINGDIALOG_HPP
