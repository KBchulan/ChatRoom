/******************************************************************************
 *
 * @file       sidebaritem.hpp
 * @brief      侧边栏的每一个元素
 *
 * @author     KBchulan
 * @date       2026/01/08
 * @history
 ******************************************************************************/

#ifndef SIDEBARITEM_HPP
#define SIDEBARITEM_HPP

#include <QLabel>
#include <QString>
#include <QWidget>

class SideBarItem : public QWidget
{
  Q_OBJECT

public:
  explicit SideBarItem(QString normal_icon, QString hover_icon, QString selected_icon, QWidget* parent = nullptr);
  ~SideBarItem() = default;

  void SetSelected(bool selected);
  void SetRedDotVisible(bool visible);

signals:
  void clicked();

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  void update_icon();

  QLabel* _icon_label;
  QLabel* _red_dot_label;

  QString _normal_icon;
  QString _hover_icon;
  QString _selected_icon;

  bool _is_selected;
  bool _is_hovered;
};

#endif  // SIDEBARITEM_HPP
