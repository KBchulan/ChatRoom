/******************************************************************************
 *
 * @file       searchuserlist.hpp
 * @brief      搜索结果的list展示
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 *****************************************************************************/

#ifndef SEARCHUSERLIST_HPP
#define SEARCHUSERLIST_HPP

#include <QEnterEvent>
#include <QEvent>
#include <QListWidget>

class SearchUserList : public QListWidget
{
  Q_OBJECT

public:
  explicit SearchUserList(QWidget* parent = nullptr);

  void AddSearchItem(const QString& uuid, const QString& head, const QString& name, const QString& desc);
  void ClearResults();

signals:
  void sig_item_clicked(const QString& uuid);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;

private:
  void addTestData();
};

#endif  // SEARCHUSERLIST_HPP
