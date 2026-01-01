/******************************************************************************
 *
 * @file       chattextedit.hpp
 * @brief      聊天输入框，自定义QTextEdit，用于输入聊天内容，主要处理鼠标进入和离开事件
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATTEXTEDIT_HPP
#define CHATTEXTEDIT_HPP

#include <QEnterEvent>
#include <QEvent>
#include <QTextEdit>

class ChatTextEdit : public QTextEdit
{
  Q_OBJECT

public:
  explicit ChatTextEdit(QWidget* parent = nullptr);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
};

#endif  // CHATTEXTEDIT_HPP
