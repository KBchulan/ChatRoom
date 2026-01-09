#include "sidebarwidget.hpp"

#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

#include "sidebaritem.hpp"

SideBarWidget::SideBarWidget(QWidget* parent)
    : QWidget{parent},
      _avatar_label(nullptr),
      _chat_item(nullptr),
      _contact_item(nullptr),
      _setting_item(nullptr),
      _current_type(SideBarItemType::Chat)
{
  setAttribute(Qt::WA_StyledBackground, true);
  init_ui();
}

void SideBarWidget::SetAvatar(const QString& avatar_path)
{
  QPixmap pixmap(avatar_path);
  pixmap = pixmap.scaled(_avatar_label->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

  QPixmap rounded(_avatar_label->size());
  rounded.fill(Qt::transparent);

  QPainter painter(&rounded);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(QBrush(pixmap));
  painter.setPen(Qt::NoPen);

  const int wid = _avatar_label->width();
  const int high = _avatar_label->height();
  const int radius = qMin(wid, high) / 5;
  painter.drawRoundedRect(0, 0, wid, high, radius, radius);

  _avatar_label->setPixmap(rounded);
}

void SideBarWidget::SetRedDotVisible(SideBarItemType type, bool visible)
{
  switch (type)
  {
    case SideBarItemType::Chat:
      _chat_item->SetRedDotVisible(visible);
      break;
    case SideBarItemType::Contact:
      _contact_item->SetRedDotVisible(visible);
      break;
    case SideBarItemType::Setting:
      _setting_item->SetRedDotVisible(visible);
      break;
  }
}

void SideBarWidget::init_ui()
{
  // 创建各个侧边栏项
  _avatar_label = new QLabel(this);
  _avatar_label->setObjectName("sidebar_avatar");
  _avatar_label->setFixedSize(40, 40);
  _avatar_label->setScaledContents(true);
  _avatar_label->setCursor(Qt::PointingHandCursor);

  // TODO: 默认头像，未来需要替换为实际的
  SetAvatar(":/avatar/avatar.ico");

  _chat_item = new SideBarItem(":/icons/chat_normal.png", ":/icons/chat_hover.png", ":/icons/chat_selected.png", this);

  _contact_item =
      new SideBarItem(":/icons/contact_normal.png", ":/icons/contact_hover.png", ":/icons/contact_selected.png", this);

  _setting_item =
      new SideBarItem(":/icons/setting_normal.png", ":/icons/setting_hover.png", ":/icons/setting_selected.png", this);

  // 默认选中聊天
  _chat_item->SetSelected(true);

  // 连接信号
  connect(_chat_item, &SideBarItem::clicked, this, [this]() { on_item_clicked(SideBarItemType::Chat); });
  connect(_contact_item, &SideBarItem::clicked, this, [this]() { on_item_clicked(SideBarItemType::Contact); });
  connect(_setting_item, &SideBarItem::clicked, this, [this]() { on_item_clicked(SideBarItemType::Setting); });

  // 布局
  auto* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(0, 20, 0, 20);
  main_layout->setSpacing(0);

  // 上部：头像、聊天、联系人
  main_layout->addWidget(_avatar_label, 0, Qt::AlignHCenter);
  main_layout->addSpacing(10);
  main_layout->addWidget(_chat_item, 0, Qt::AlignHCenter);
  main_layout->addWidget(_contact_item, 0, Qt::AlignHCenter);

  // 中间 spacer
  main_layout->addStretch();

  // 下部：设置
  main_layout->addWidget(_setting_item, 0, Qt::AlignHCenter);
}

void SideBarWidget::on_item_clicked(SideBarItemType type)
{
  if (type == SideBarItemType::Setting)
  {
    emit itemChanged(type);
    return;
  }

  if (_current_type == type)
  {
    return;
  }

  // 取消之前的选中
  switch (_current_type)
  {
    case SideBarItemType::Chat:
      _chat_item->SetSelected(false);
      break;
    case SideBarItemType::Contact:
      _contact_item->SetSelected(false);
      break;
    default:
      break;
  }

  // 设置新的选中
  _current_type = type;
  switch (type)
  {
    case SideBarItemType::Chat:
      _chat_item->SetSelected(true);
      break;
    case SideBarItemType::Contact:
      _contact_item->SetSelected(true);
      break;
    default:
      break;
  }

  emit itemChanged(type);
}
