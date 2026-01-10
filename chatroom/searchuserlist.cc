#include "searchuserlist.hpp"

#include <QScrollBar>
#include <QTimer>

#include "global.hpp"
#include "loadingitem.hpp"
#include "searchuseritem.hpp"

SearchUserList::SearchUserList(QWidget* parent) : QListWidget(parent)
{
  // 默认隐藏滚动条
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // 点击事件：获取 uuid 并发出信号
  connect(this, &QListWidget::itemClicked,
          [this](QListWidgetItem* item)
          {
            auto* widget = qobject_cast<SearchUserItem*>(itemWidget(item));
            if (widget != nullptr)
            {
              emit sig_item_clicked(widget->GetUuid());
            }
          });
}

void SearchUserList::AddSearchItem(const QString& uuid, const QString& head, const QString& name, const QString& desc)
{
  auto* item = new QListWidgetItem();
  item->setSizeHint(QSize(0, UIConstants::SearchUserItemHeight));

  auto* widget = new SearchUserItem();
  widget->UpdateItem(uuid, head, name, desc);

  addItem(item);
  setItemWidget(item, widget);
}

void SearchUserList::StartSearch()
{
  if (_loading)
  {
    return;
  }

  ClearResults();
  showLoading();

  // TODO: 替换为真实网络请求
  QTimer::singleShot(1000, this,
                     [this]()
                     {
                       hideLoading();

                       // TODO: 模拟成功，后续根据实际响应处理
                       bool success = true;
                       if (success)
                       {
                         addTestData();
                       }
                       else
                       {
                         emit sig_search_failed();
                       }
                     });
}

void SearchUserList::ClearResults()
{
  clear();
}

void SearchUserList::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QListWidget::enterEvent(event);
}

void SearchUserList::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QListWidget::leaveEvent(event);
}

void SearchUserList::showLoading()
{
  if (_loading)
  {
    return;
  }

  _loading = true;
  _loading_item = new QListWidgetItem();
  _loading_item->setSizeHint(QSize(0, UIConstants::LoadingItemHeight));

  auto* widget = new LoadingItem("搜索用户中...");
  addItem(_loading_item);
  setItemWidget(_loading_item, widget);
}

void SearchUserList::hideLoading()
{
  if (_loading_item != nullptr)
  {
    delete itemWidget(_loading_item);
    delete takeItem(row(_loading_item));
    _loading_item = nullptr;
  }
  _loading = false;
}

void SearchUserList::addTestData()
{
  const QStringList heads = {":/avatar/head_1.jpg", ":/avatar/head_2.jpg", ":/avatar/head_3.jpg"};

  const QStringList names = {"Alice", "Bob", "Charlie", "David", "Eva"};

  const QStringList descs = {"前端开发工程师", "后端架构师", "全栈开发者", "UI/UX 设计师", "产品经理"};

  for (int i = 0; i < 15; ++i)
  {
    QString uuid = QString("user_%1").arg(i + 1);
    AddSearchItem(uuid, heads[i % 3], names[i % 5], descs[i % 5]);
  }
}
