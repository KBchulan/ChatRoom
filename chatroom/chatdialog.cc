#include "chatdialog.hpp"

#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "chatpage.hpp"
#include "chatuseritem.hpp"
#include "chatuserlist.hpp"
#include "global.hpp"
#include "loadingitem.hpp"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ChatDialog), _loading(false), _loading_item(nullptr)
{
  ui->setupUi(this);

  /* 搜索框: 默认文本，最大长度，左侧搜索，右侧清除 */
  ui->search_edit->setPlaceholderText("搜索");
  ui->search_edit->setMaxLength(15);

  // 左侧搜索图标
  _search_action = ui->search_edit->addAction(QIcon(":/icons/search.png"), QLineEdit::LeadingPosition);

  // 右侧清除图标
  _clear_action = ui->search_edit->addAction(QIcon(":/icons/chahao.png"), QLineEdit::TrailingPosition);
  _clear_action->setVisible(false);

  // 设置两个按钮为 小手 图标
  for (auto* btn : ui->search_edit->findChildren<QToolButton*>())
  {
    btn->setCursor(Qt::PointingHandCursor);
  }

  // 文本变化时切换清除按钮显示/隐藏
  connect(ui->search_edit, &QLineEdit::textChanged,
          [this](const QString& text) { _clear_action->setVisible(!text.isEmpty()); });

  // 点击清除按钮清空文本 并 移除焦点
  connect(_clear_action, &QAction::triggered,
          [this]()
          {
            ui->search_edit->clear();
            ui->search_edit->clearFocus();
          });

  /* 中间主页面 */
  auto* stack_layout_1 = new QVBoxLayout(ui->mid_stack_widget);
  stack_layout_1->setContentsMargins(0, 0, 1, 0);
  stack_layout_1->setSpacing(0);

  // 加载所有可能的页面，并设置默认页面
  _mid_stacked_widget = new QStackedWidget();
  _chat_user_list = new ChatUserList();
  _mid_stacked_widget->addWidget(_chat_user_list);
  _mid_stacked_widget->setCurrentWidget(_chat_user_list);
  stack_layout_1->addWidget(_mid_stacked_widget);

  /* 右侧主页面 */
  auto* stack_layout_2 = new QVBoxLayout(ui->right_stack_widget);
  stack_layout_2->setContentsMargins(0, 0, 0, 0);
  stack_layout_2->setSpacing(0);

  // 加载所有可能的页面，并设置默认页面
  _right_stacked_widget = new QStackedWidget();
  _chat_page = new ChatPage();
  _right_stacked_widget->addWidget(_chat_page);
  _right_stacked_widget->setCurrentWidget(_chat_page);
  stack_layout_2->addWidget(_right_stacked_widget);

  /* 连接所有的槽和信号 */
  connect(_chat_user_list, &ChatUserList::sig_load_chat_user, this, &ChatDialog::slot_load_chat_user);

  // 测试数据
  addTestData();
}

ChatDialog::~ChatDialog()
{
  delete ui;
}

void ChatDialog::addTestData()
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
    auto* item = new QListWidgetItem();
    item->setSizeHint(QSize(0, UIConstants::ChatUserItemHeight));

    auto* widget = new ChatUserItem();
    widget->UpdateItem(heads[i % 3], names[i], msgs[i], times[i % 7]);

    _chat_user_list->addItem(item);
    _chat_user_list->setItemWidget(item, widget);
  }
}

void ChatDialog::slot_load_chat_user()
{
  if (_loading)
  {
    return;
  }

  _loading = true;
  showLoading();

  // TODO: 模拟网络延迟，后续替换为真实请求
  QTimer::singleShot(1000, this,
                     [this]()
                     {
                       hideLoading();
                       addTestData();
                     });
}

void ChatDialog::showLoading()
{
  _loading_item = new QListWidgetItem();
  _loading_item->setSizeHint(QSize(0, UIConstants::LoadingItemHeight));

  auto* widget = new LoadingItem("加载用户数据中...");
  _chat_user_list->addItem(_loading_item);
  _chat_user_list->setItemWidget(_loading_item, widget);

  _chat_user_list->scrollToBottom();
}

void ChatDialog::hideLoading()
{
  if (_loading_item != nullptr)
  {
    delete _chat_user_list->itemWidget(_loading_item);
    delete _chat_user_list->takeItem(_chat_user_list->row(_loading_item));
    _loading_item = nullptr;
  }
  _loading = false;
}
