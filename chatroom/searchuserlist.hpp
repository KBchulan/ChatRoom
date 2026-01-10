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
#include <QListWidgetItem>

class SearchUserList : public QListWidget
{
  Q_OBJECT

public:
  explicit SearchUserList(QWidget* parent = nullptr);

  void AddSearchItem(const QString& uuid, const QString& head, const QString& name, const QString& desc);
  void ClearResults();
  void StartSearch();

signals:
  void sig_item_clicked(const QString& uuid);
  void sig_search_failed();

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;

private:
  void showLoading();
  void hideLoading();
  void addTestData();

  bool _loading = false;
  QListWidgetItem* _loading_item = nullptr;
};

#endif  // SEARCHUSERLIST_HPP
