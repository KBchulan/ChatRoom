#include "chatdialog.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QMouseEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include "chatpage.hpp"
#include "chatuserlist.hpp"
#include "contactlist.hpp"
#include "findfaileddialog.hpp"
#include "findsuccessdialog.hpp"
#include "friendapplypage.hpp"
#include "searchuserlist.hpp"
#include "settingdialog.hpp"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ChatDialog), _friend_apply_page(nullptr)
{
  ui->setupUi(this);
  _setting_dialog = new SettingDialog(this);

  // 获取应用程序静态资源目录
  QString app_dir = QCoreApplication::applicationDirPath();
  _static_dir = app_dir + QDir::separator() + "static";

  /* 搜索框: 默认文本，最大长度，左侧搜索，右侧清除 */
  ui->search_edit->setPlaceholderText("请输入用户名或邮箱搜索");
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

  // 设置 add_button 为 小手 图标
  ui->add_button->setCursor(Qt::PointingHandCursor);

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
  _contact_page = new QWidget();
  _right_stacked_widget->addWidget(_chat_page);
  _right_stacked_widget->addWidget(_contact_page);
  _right_stacked_widget->setCurrentWidget(_chat_page);
  stack_layout_2->addWidget(_right_stacked_widget);

  /* 连接信号和槽 */
  connect(ui->chat_side_bar_widget, &SideBarWidget::itemChanged, this, &ChatDialog::slot_item_changed);
  connect(_search_user_list, &SearchUserList::sig_item_clicked, this, &ChatDialog::slot_search_item_clicked);
  connect(_search_user_list, &SearchUserList::sig_search_failed, this, &ChatDialog::slot_search_failed);
  connect(_contact_list, &ContactList::sig_new_friends_clicked, this, &ChatDialog::slot_new_friends_clicked);
  connect(_chat_page, &ChatPage::sig_clicked, this,
          [this]()
          {
            ui->search_edit->clear();
            ui->search_edit->clearFocus();
          });

  // 安装事件过滤器
  this->installEventFilter(this);
}

ChatDialog::~ChatDialog()
{
  delete ui;
}

bool ChatDialog::eventFilter(QObject* watched, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    auto* mouse_event = static_cast<QMouseEvent*>(event);
    auto click_pos = mouse_event->globalPosition().toPoint();

    // 获取 chat_user_widget 的全局区域
    auto user_rect = ui->chat_user_widget->rect();
    auto user_global_rect = QRect(ui->chat_user_widget->mapToGlobal(user_rect.topLeft()), user_rect.size());

    // 如果点击位置不在 chat_user_widget 区域内，清空输入框
    if (!user_global_rect.contains(click_pos))
    {
      ui->search_edit->clear();
      ui->search_edit->clearFocus();
    }
  }

  return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleTextChange(const QString& text)
{
  auto is_search = !text.isEmpty();
  _clear_action->setVisible(is_search);

  // 只在清空文本时回到上一个页面
  if (!is_search && _mid_stacked_widget->currentWidget() == _search_user_list)
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
      _right_stacked_widget->setCurrentWidget(_chat_page);
      break;
    case SideBarItemType::Contact:
      target = _contact_list;
      _right_stacked_widget->setCurrentWidget(_contact_page);
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

void ChatDialog::slot_search_item_clicked(const QString& uuid)
{
  // TODO: 根据 uuid 获取完整用户信息
  auto* dialog = new FindSuccessDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  auto head = _static_dir + QDir::separator() + "test1.ico";
  dialog->SetUserInfo(head, "test_user_" + uuid);
  dialog->exec();
}

void ChatDialog::slot_search_failed()
{
  auto* dialog = new FindFailedDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec();
  ui->search_edit->clear();
  ui->search_edit->clearFocus();
}

void ChatDialog::on_add_button_clicked()
{
  // 记录当前页面并切换到搜索页面
  if (_mid_stacked_widget->currentWidget() != _search_user_list)
  {
    _prev_mid_widget = _mid_stacked_widget->currentWidget();
    _mid_stacked_widget->setCurrentWidget(_search_user_list);
  }

  // 发起搜索好友请求，把搜索结果显示在 SearchUserList 中
  _search_user_list->StartSearch();
}

void ChatDialog::slot_new_friends_clicked()
{
  if (_friend_apply_page == nullptr)
  {
    _friend_apply_page = new FriendApplyPage();
    _right_stacked_widget->addWidget(_friend_apply_page);
  }

  _contact_page = _friend_apply_page;
  _right_stacked_widget->setCurrentWidget(_contact_page);
}
