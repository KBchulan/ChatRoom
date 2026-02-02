/******************************************************************************
 *
 * @file       customlistwidget.hpp
 * @brief      自定义 ListWidget 类，做通用基类使用
 *
 * @author     KBchulan
 * @date       2026/02/02
 * @history
 ******************************************************************************/

#ifndef CUSTOMLISTWIDGET_HPP
#define CUSTOMLISTWIDGET_HPP

#include <QEnterEvent>
#include <QEvent>
#include <QListWidget>
#include <QListWidgetItem>

class CustomListWidget : public QListWidget
{
public:
  explicit CustomListWidget(QWidget* parent = nullptr);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void showEvent(QShowEvent* event) override;
};

#endif  // CUSTOMLISTWIDGET_HPP
