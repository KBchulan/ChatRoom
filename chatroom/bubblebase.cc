#include "bubblebase.hpp"

#include <QPainter>
#include <QPainterPath>

BubbleBase::BubbleBase(MsgDirection direction, QWidget* parent) : QFrame(parent), _direction(direction)
{
  setObjectName(_direction == MsgDirection::Sent ? "bubble_sent" : "bubble_received");
  setAttribute(Qt::WA_StyledBackground, true);
}

QMargins BubbleBase::contentMargins() const
{
  const int triangle_width = UIConstants::ChatMsgItemTriangleWidth;
  const int padding = 10;
  const int v_padding = 8;

  if (_direction == MsgDirection::Sent)
  {
    // 右侧有三角，右边距需要加上三角宽度
    return {padding, v_padding, padding + triangle_width, v_padding};
  }
  // 左侧有三角，左边距需要加上三角宽度
  return {padding + triangle_width, v_padding, padding, v_padding};
}

void BubbleBase::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event)

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const int radius = UIConstants::ChatMsgItemBubbleRadius;
  const int triangle_width = UIConstants::ChatMsgItemTriangleWidth;
  const int triangle_height = 12;

  // 获取气泡颜色
  QColor bubble_color = palette().color(QPalette::Window);

  painter.setPen(Qt::NoPen);
  painter.setBrush(bubble_color);

  QPainterPath path;

  if (_direction == MsgDirection::Sent)
  {
    // 自己发送：三角在右侧
    QRect bubble_rect(0, 0, width() - triangle_width, height());
    path.addRoundedRect(bubble_rect, radius, radius);

    // 右侧小三角
    int tri_x = width() - triangle_width;
    int tri_y = 15;
    path.moveTo(tri_x, tri_y);
    path.lineTo(tri_x + triangle_width, tri_y + (triangle_height / 2));
    path.lineTo(tri_x, tri_y + triangle_height);
    path.closeSubpath();
  }
  else
  {
    // 对方消息：三角在左侧
    QRect bubble_rect(triangle_width, 0, width() - triangle_width, height());
    path.addRoundedRect(bubble_rect, radius, radius);

    // 左侧小三角
    int tri_x = triangle_width;
    int tri_y = 15;
    path.moveTo(tri_x, tri_y);
    path.lineTo(0, tri_y + (triangle_height / 2));
    path.lineTo(tri_x, tri_y + triangle_height);
    path.closeSubpath();
  }

  painter.drawPath(path);
}
