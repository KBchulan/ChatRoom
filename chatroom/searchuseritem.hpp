/******************************************************************************
 *
 * @file       searchuseritem.hpp
 * @brief      搜索页面的单个 item 设计
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 *****************************************************************************/

#ifndef SEARCHUSERITEM_HPP
#define SEARCHUSERITEM_HPP

#include <QString>
#include <QWidget>

namespace Ui
{
class SearchUserItem;
}

class SearchUserItem : public QWidget
{
  Q_OBJECT

public:
  explicit SearchUserItem(QWidget* parent = nullptr);
  ~SearchUserItem() override;

  void SetHead(const QString& head);
  void SetName(const QString& name);
  void SetDesc(const QString& desc);
  void SetUuid(const QString& uuid);

  void UpdateItem(const QString& uuid, const QString& head, const QString& name, const QString& desc);

  [[nodiscard]] QString GetUuid() const;

private:
  Ui::SearchUserItem* ui;
  QString _uuid;
};

#endif  // SEARCHUSERITEM_HPP
