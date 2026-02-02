/******************************************************************************
 *
 * @file       friendapplypage.hpp
 * @brief      好友申请页面
 *
 * @author     KBchulan
 * @date       2026/01/20
 * @history
 ******************************************************************************/

#ifndef FRIENDAPPLYPAGE_HPP
#define FRIENDAPPLYPAGE_HPP

#include <QWidget>

namespace Ui
{
class FriendApplyPage;
}

class FriendApplyPage : public QWidget
{
  Q_OBJECT

public:
  explicit FriendApplyPage(QWidget* parent = nullptr);
  ~FriendApplyPage();

private:
  void initUI();

  Ui::FriendApplyPage* ui;
};

#endif  // FRIENDAPPLYPAGE_HPP
