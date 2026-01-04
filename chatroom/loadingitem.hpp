/******************************************************************************
 *
 * @file       loadingitem.hpp
 * @brief      单个加载组件，用于显示加载动画，注入 ChatUserList 里
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef LOADINGITEM_HPP
#define LOADINGITEM_HPP

#include <QMovie>
#include <QString>
#include <QWidget>

namespace Ui
{
class LoadingItem;
}

class LoadingItem : public QWidget
{
  Q_OBJECT

public:
  explicit LoadingItem(const QString& tip, QWidget* parent = nullptr);
  ~LoadingItem();

private:
  Ui::LoadingItem* ui;
  QMovie* _movie;
};

#endif  // LOADINGITEM_HPP
