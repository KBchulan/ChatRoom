/******************************************************************************
 *
 * @file       bubblebase.hpp
 * @brief      聊天气泡基类，负责绘制带小三角的气泡外形，子类实现具体内容
 *
 * @author     KBchulan
 * @date       2026/01/02
 * @history
 ******************************************************************************/

#ifndef BUBBLEBASE_HPP
#define BUBBLEBASE_HPP

#include <QFrame>

#include "global.hpp"

class BubbleBase : public QFrame
{
  Q_OBJECT

public:
  explicit BubbleBase(MsgDirection direction, QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;

  // 子类需要调用此方法获取内容区域的边距
  [[nodiscard]] QMargins contentMargins() const;

private:
  MsgDirection _direction;
};

#endif  // BUBBLEBASE_HPP
