#include "sidebaritem.hpp"

#include <QMouseEvent>
#include <QVBoxLayout>

SideBarItem::SideBarItem(QString normal_icon, QString hover_icon, QString selected_icon, QWidget* parent)
    : QWidget(parent),
      _icon_label(new QLabel(this)),
      _red_dot_label(new QLabel(this)),
      _normal_icon(std::move(normal_icon)),
      _hover_icon(std::move(hover_icon)),
      _selected_icon(std::move(selected_icon)),
      _is_selected(false),
      _is_hovered(false)
{
  setFixedSize(50, 50);
  setCursor(Qt::PointingHandCursor);

  // 图标标签
  _icon_label->setObjectName("sidebar_icon");
  _icon_label->setFixedSize(25, 25);
  _icon_label->setScaledContents(true);

  // 小红点标签，放在图标右上角
  _red_dot_label->setObjectName("sidebar_red_dot");
  _red_dot_label->setFixedSize(25, 25);
  _red_dot_label->setScaledContents(true);
  _red_dot_label->setPixmap(QPixmap(":/icons/red_dot.png"));
  _red_dot_label->move(17, 8);
  _red_dot_label->hide();

  // 布局
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(Qt::AlignCenter);
  layout->addWidget(_icon_label, 0, Qt::AlignCenter);

  update_icon();
}

void SideBarItem::SetSelected(bool selected)
{
  if (_is_selected != selected)
  {
    _is_selected = selected;
    update_icon();
  }
}

void SideBarItem::SetRedDotVisible(bool visible)
{
  _red_dot_label->setVisible(visible);
}

void SideBarItem::enterEvent(QEnterEvent* event)
{
  _is_hovered = true;
  update_icon();
  QWidget::enterEvent(event);
}

void SideBarItem::leaveEvent(QEvent* event)
{
  _is_hovered = false;
  update_icon();
  QWidget::leaveEvent(event);
}

void SideBarItem::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    emit clicked();
  }
  QWidget::mousePressEvent(event);
}

void SideBarItem::update_icon()
{
  QString icon_path;
  if (_is_selected)
  {
    icon_path = _selected_icon;
  }
  else if (_is_hovered)
  {
    icon_path = _hover_icon;
  }
  else
  {
    icon_path = _normal_icon;
  }

  _icon_label->setPixmap(QPixmap(icon_path));
}
