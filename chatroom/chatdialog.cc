#include "chatdialog.hpp"

#include <QToolButton>
#include <QVBoxLayout>

#include "chatpage.hpp"
#include "chatuserlist.hpp"
#include "contactlist.hpp"
#include "searchuserlist.hpp"
#include "settingdialog.hpp"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ChatDialog)
{
  ui->setupUi(this);
  _setting_dialog = new SettingDialog(this);

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
  connect(ui->search_edit, &QLineEdit::textChanged, [this](const QString& str) { handleTextChange(str); });

  // 点击清除按钮清空文本 并 移除焦点
  connect(_clear_action, &QAction::triggered,
          [this]()
          {
            ui->search_edit->clear();
            ui->search_edit->clearFocus();
          });

  /* 中间主页面 */
  auto* stack_layout_1 = new QVBoxLayout(ui->mid_stack_widget);
  stack_layout_1->setContentsMargins(1, 0, 1, 0);
  stack_layout_1->setSpacing(0);

  // 加载所有可能的页面，并设置默认页面
  _mid_stacked_widget = new QStackedWidget();
  _chat_user_list = new ChatUserList();
  _search_user_list = new SearchUserList();
  _contact_list = new ContactList();
  _mid_stacked_widget->addWidget(_chat_user_list);
  _mid_stacked_widget->addWidget(_search_user_list);
  _mid_stacked_widget->addWidget(_contact_list);

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

  /* 连接信号和槽 */
  connect(ui->chat_side_bar_widget, &SideBarWidget::itemChanged, this, &ChatDialog::slot_item_changed);
}

ChatDialog::~ChatDialog()
{
  delete ui;
}

void ChatDialog::handleTextChange(const QString& text)
{
  auto is_search = !text.isEmpty();
  _clear_action->setVisible(is_search);

  if (is_search)
  {
    if (_prev_mid_widget == nullptr)
    {
      _prev_mid_widget = _mid_stacked_widget->currentWidget();
    }
    _mid_stacked_widget->setCurrentWidget(_search_user_list);
  }
  else
  {
    _mid_stacked_widget->setCurrentWidget(_prev_mid_widget != nullptr ? _prev_mid_widget : _chat_user_list);
    _prev_mid_widget = nullptr;
  }
}

void ChatDialog::slot_item_changed(SideBarItemType type)
{
  if (type == SideBarItemType::Setting)
  {
    _setting_dialog->exec();
    return;
  }

  QWidget* target = nullptr;
  switch (type)
  {
    case SideBarItemType::Chat:
      target = _chat_user_list;
      break;
    case SideBarItemType::Contact:
      target = _contact_list;
      break;
    default:
      return;
  }

  // 如果当前在搜索，更新返回目标；否则直接切换
  if (_mid_stacked_widget->currentWidget() == _search_user_list)
  {
    _prev_mid_widget = target;
  }
  else
  {
    _mid_stacked_widget->setCurrentWidget(target);
  }
}
