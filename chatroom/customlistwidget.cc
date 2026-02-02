#include "customlistwidget.hpp"

#include <QTimer>

CustomListWidget::CustomListWidget(QWidget* parent) : QListWidget(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void CustomListWidget::enterEvent(QEnterEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QListWidget::enterEvent(event);
}

void CustomListWidget::leaveEvent(QEvent* event)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QListWidget::leaveEvent(event);
}

void CustomListWidget::showEvent(QShowEvent* event)
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
