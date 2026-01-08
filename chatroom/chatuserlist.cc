#include "chatuserlist.hpp"

#include <QScrollBar>
#include <QShowEvent>

#include "chatuseritem.hpp"
#include "global.hpp"
#include "loadingitem.hpp"

ChatUserList::ChatUserList(QWidget* parent) : QListWidget(parent)
{
  // 默认隐藏滚动条
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  _debounce_timer = new QTimer(this);
  _debounce_timer->setSingleShot(true);
  _debounce_timer->setInterval(200);

  connect(_debounce_timer, &QTimer::timeout, this, &ChatUserList::slot_load_more);

  // 滚动到底部时触发加载
  connect(verticalScrollBar(), &QScrollBar::valueChanged,
          [this](int value)
          {
            if (value == verticalScrollBar()->maximum() && !_debounce_timer->isActive())
            {
              _debounce_timer->start();
            }
          });

  // 初始化测试数据
  addTestData();
}

void ChatUserList::addUserItem(const QString& head, const QString& name, const QString& msg, const QString& time)
{
  auto* item = new QListWidgetItem();
  item->setSizeHint(QSize(0, UIConstants::ChatUserItemHeight));

  auto* widget = new ChatUserItem();
  widget->UpdateItem(head, name, msg, time);

  addItem(item);
  setItemWidget(item, widget);
}

void ChatUserList::slot_load_more()
{
  if (_loading)
  {
    return;
  }

  showLoading();

  // TODO: 模拟网络延迟，后续替换为真实请求
  QTimer::singleShot(1000, this,
                     [this]()
                     {
                       hideLoading();
                       addTestData();
                     });
}

void ChatUserList::showLoading()
{
  if (_loading)
  {
    return;
  }

  _loading = true;
  _loading_item = new QListWidgetItem();
  _loading_item->setSizeHint(QSize(0, UIConstants::LoadingItemHeight));

  auto* widget = new LoadingItem("加载用户数据中...");
  addItem(_loading_item);
  setItemWidget(_loading_item, widget);

  scrollToBottom();
}

void ChatUserList::hideLoading()
{
  if (_loading_item != nullptr)
  {
    delete itemWidget(_loading_item);
    delete takeItem(row(_loading_item));
    _loading_item = nullptr;
  }
  _loading = false;
}

void ChatUserList::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QListWidget::enterEvent(event);
}

void ChatUserList::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QListWidget::leaveEvent(event);
}

void ChatUserList::showEvent(QShowEvent* event)
{
  QListWidget::showEvent(event);
  // 刷新布局，此为必要项，不要删除
  QTimer::singleShot(0, this,
                     [this]()
                     {
                       setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                       setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                     });
}

void ChatUserList::addTestData()
{
  const QStringList heads = {":/avatar/head_1.jpg", ":/avatar/head_2.jpg", ":/avatar/head_3.jpg"};

  const QStringList names = {"张三", "李四", "王五", "赵六", "孙七", "周八",
                             "吴九", "郑十", "小明", "小红", "小刚", "小丽"};

  const QStringList msgs = {"今天天气真好！", "晚上一起吃饭吗？", "项目进度怎么样了？", "[图片]",
                            "好的，收到",     "我马上就到",       "[语音消息]",         "明天见～",
                            "哈哈哈哈",       "在吗？",           "周末有空吗？",       "OK"};

  const QStringList times = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};

  for (int i = 0; i < 12; ++i)
  {
    addUserItem(heads[i % 3], names[i], msgs[i], times[i % 7]);
  }
}
