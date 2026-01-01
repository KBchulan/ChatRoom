/******************************************************************************
 *
 * @file       chatpage.hpp
 * @brief      聊天页面最右侧组件，显示聊天的主体内容
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef CHATPAGE_HPP
#define CHATPAGE_HPP

#include <QWidget>

namespace Ui
{
class ChatPage;
}

class ChatPage : public QWidget
{
  Q_OBJECT

public:
  explicit ChatPage(QWidget* parent = nullptr);
  ~ChatPage();

private:
  void addTestData();

  Ui::ChatPage* ui;
};

#endif  // CHATPAGE_HPP
