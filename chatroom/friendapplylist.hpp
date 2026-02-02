/******************************************************************************
 *
 * @file       friendapplylist.hpp
 * @brief      新的好友申请列表
 *
 * @author     KBchulan
 * @date       2026/02/02
 * @history
 ******************************************************************************/

#ifndef FRIENDAPPLYLIST_HPP
#define FRIENDAPPLYLIST_HPP

#include <QShowEvent>

#include "customlistwidget.hpp"
#include "friendapplyitem.hpp"

class FriendApplyList : public CustomListWidget
{
  Q_OBJECT

public:
  explicit FriendApplyList(QWidget* parent = nullptr);

  // 添加申请项，自动插入到正确位置
  FriendApplyItem* addApplyItem(const QString& head, const QString& name, const QString& msg, ApplyStatus status,
                                std::int64_t timestamp);

private slots:
  void slot_accept_clicked(FriendApplyItem* apply_item);

private:
  void addTestData();

  // 找到插入位置，保持排序：Pending在前按时间降序，Accepted在后按时间降序
  [[nodiscard]] int findInsertPosition(ApplyStatus status, std::int64_t timestamp) const;
};

#endif  // FRIENDAPPLYLIST_HPP
