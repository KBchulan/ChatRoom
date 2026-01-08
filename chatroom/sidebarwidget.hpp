/******************************************************************************
 *
 * @file       sidebarwidget.hpp
 * @brief      聊天页面的侧边栏
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef SIDEBARWIDGET_HPP
#define SIDEBARWIDGET_HPP

#include <QLabel>
#include <QWidget>
#include <cstdint>

// 侧边栏功能项枚举
enum class SideBarItemType : std::uint8_t
{
  Chat,     // 聊天
  Contact,  // 联系人
  Setting   // 设置
};

class SideBarItem;
class SideBarWidget : public QWidget
{
  Q_OBJECT
public:
  explicit SideBarWidget(QWidget* parent = nullptr);

  void SetAvatar(const QString& avatar_path);
  void SetRedDotVisible(SideBarItemType type, bool visible);

signals:
  // 导航项切换
  void itemChanged(SideBarItemType type);

private:
  void init_ui();
  void on_item_clicked(SideBarItemType type);

  QLabel* _avatar_label;
  SideBarItem* _chat_item;
  SideBarItem* _contact_item;
  SideBarItem* _setting_item;

  SideBarItemType _current_type;
};

#endif  // SIDEBARWIDGET_HPP
