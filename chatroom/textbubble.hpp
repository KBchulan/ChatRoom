/******************************************************************************
 *
 * @file       textbubble.hpp
 * @brief      文本消息气泡，继承 BubbleBase
 *
 * @author     KBchulan
 * @date       2026/01/02
 * @history
 ******************************************************************************/

#ifndef TEXTBUBBLE_HPP
#define TEXTBUBBLE_HPP

#include "bubblebase.hpp"

class QLabel;
class TextBubble : public BubbleBase
{
  Q_OBJECT

public:
  explicit TextBubble(MsgDirection direction, const QString& text = QString(), QWidget* parent = nullptr);

  void setText(const QString& text);

private:
  void initUI();

  QLabel* _text_label;
};

#endif  // TEXTBUBBLE_HPP
