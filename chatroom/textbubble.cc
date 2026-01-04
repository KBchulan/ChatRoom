#include "textbubble.hpp"

#include <QHBoxLayout>
#include <QLabel>

TextBubble::TextBubble(MsgDirection direction, const QString& text, QWidget* parent) : BubbleBase(direction, parent)
{
  initUI();

  if (!text.isEmpty())
  {
    setText(text);
  }
}

void TextBubble::initUI()
{
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(contentMargins());

  _text_label = new QLabel(this);
  _text_label->setObjectName("bubble_text");
  _text_label->setWordWrap(true);
  _text_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  layout->addWidget(_text_label);
}

void TextBubble::setText(const QString& text)
{
  _text_label->setText(text);
  adjustSize();
}
