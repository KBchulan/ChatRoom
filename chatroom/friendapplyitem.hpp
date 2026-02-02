/******************************************************************************
 *
 * @file       friendapplyitem.hpp
 * @brief      新的好友申请单个 item
 *
 * @author     KBchulan
 * @date       2026/02/02
 * @history
 ******************************************************************************/

#ifndef FRIENDAPPLYITEM_HPP
#define FRIENDAPPLYITEM_HPP

#include <QWidget>
#include <cstdint>

namespace Ui
{
class FriendApplyItem;
}

// 好友申请状态
enum class ApplyStatus : std::uint8_t
{
  Pending = 0,   // 待处理
  Accepted = 1,  // 已接受
};

class FriendApplyItem : public QWidget
{
  Q_OBJECT

public:
  explicit FriendApplyItem(QWidget* parent = nullptr);
  ~FriendApplyItem() override;

  void SetHead(const QString& head);
  void SetName(const QString& name);
  void SetMsg(const QString& msg);
  void SetStatus(ApplyStatus status);
  void SetTimestamp(std::int64_t timestamp);

  [[nodiscard]] const QString& GetHead() const
  {
    return _head;
  }
  [[nodiscard]] const QString& GetName() const
  {
    return _name;
  }
  [[nodiscard]] const QString& GetMsg() const
  {
    return _msg;
  }
  [[nodiscard]] ApplyStatus GetStatus() const
  {
    return _status;
  }
  [[nodiscard]] std::int64_t GetTimestamp() const
  {
    return _timestamp;
  }

signals:
  void sig_accept_clicked(FriendApplyItem* item);

private slots:
  void slot_accept_clicked();

private:
  Ui::FriendApplyItem* ui;
  QString _head;
  QString _name;
  QString _msg;
  ApplyStatus _status = ApplyStatus::Pending;
  std::int64_t _timestamp = 0;
};

#endif  // FRIENDAPPLYITEM_HPP
