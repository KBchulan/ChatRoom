#include "friendapplylist.hpp"

#include <QScrollBar>
#include <QTimer>

#include "global.hpp"

FriendApplyList::FriendApplyList(QWidget* parent) : CustomListWidget(parent)
{
  addTestData();
}

FriendApplyItem* FriendApplyList::addApplyItem(const QString& head, const QString& name, const QString& msg,
                                               ApplyStatus status, std::int64_t timestamp)
{
  auto* list_item = new QListWidgetItem();
  list_item->setSizeHint(QSize(0, UIConstants::FriendApplyItemHeight));

  auto* widget = new FriendApplyItem();
  widget->SetHead(head);
  widget->SetName(name);
  widget->SetMsg(msg);
  widget->SetStatus(status);
  widget->SetTimestamp(timestamp);

  // 找到正确的插入位置
  int pos = findInsertPosition(status, timestamp);
  insertItem(pos, list_item);
  setItemWidget(list_item, widget);

  connect(widget, &FriendApplyItem::sig_accept_clicked, this, &FriendApplyList::slot_accept_clicked);

  return widget;
}

int FriendApplyList::findInsertPosition(ApplyStatus status, std::int64_t timestamp) const
{
  for (int i = 0; i < count(); ++i)
  {
    auto* widget = qobject_cast<FriendApplyItem*>(itemWidget(item(i)));
    if (widget == nullptr)
    {
      continue;
    }

    ApplyStatus item_status = widget->GetStatus();
    std::int64_t item_timestamp = widget->GetTimestamp();

    // 如果新项是 Pending
    if (status == ApplyStatus::Pending)
    {
      // 遇到 Accepted 项，插在它前面
      if (item_status == ApplyStatus::Accepted)
      {
        return i;
      }
      // 同为 Pending，比较时间戳，新的在前
      if (timestamp > item_timestamp)
      {
        return i;
      }
    }
    // 如果新项是 Accepted
    else
    {
      // 跳过所有 Pending 项
      if (item_status == ApplyStatus::Pending)
      {
        continue;
      }
      // 同为 Accepted，比较时间戳，新的在前
      if (timestamp > item_timestamp)
      {
        return i;
      }
    }
  }

  return count();
}

void FriendApplyList::slot_accept_clicked(FriendApplyItem* apply_item)
{
  // TODO: 这里需要发送接受好友申请的网络请求，成功后再更新状态

  // 保存数据
  QString head = apply_item->GetHead();
  QString name = apply_item->GetName();
  QString msg = apply_item->GetMsg();
  std::int64_t timestamp = apply_item->GetTimestamp();

  // 找到并删除该 item
  for (int i = 0; i < count(); ++i)
  {
    if (itemWidget(item(i)) == apply_item)
    {
      delete takeItem(i);
      break;
    }
  }

  // 重新添加，状态为 Accepted
  addApplyItem(head, name, msg, ApplyStatus::Accepted, timestamp);
}

void FriendApplyList::addTestData()
{
  const QStringList heads = {":/avatar/head_1.jpg", ":/avatar/head_2.jpg", ":/avatar/head_3.jpg"};

  struct TestItem
  {
    QString name;
    QString msg;
    ApplyStatus status;
    std::int64_t timestamp;
  };

  const QList<TestItem> test_items = {
      {.name = "张三", .msg = "你好，我是张三", .status = ApplyStatus::Pending, .timestamp = 1738400000},
      {.name = "李四", .msg = "加个好友吧", .status = ApplyStatus::Accepted, .timestamp = 1738300000},
      {.name = "王五", .msg = "我们是同事", .status = ApplyStatus::Pending, .timestamp = 1738350000},
      {.name = "赵六", .msg = "认识一下", .status = ApplyStatus::Accepted, .timestamp = 1738200000},
      {.name = "孙七", .msg = "朋友推荐", .status = ApplyStatus::Pending, .timestamp = 1738450000},
  };

  for (int i = 0; i < test_items.size(); ++i)
  {
    const auto& test_item = test_items[i];
    addApplyItem(heads[i % 3], test_item.name, test_item.msg, test_item.status, test_item.timestamp);
  }
}
