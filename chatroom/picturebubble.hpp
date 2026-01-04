/******************************************************************************
 *
 * @file       picturebubble.hpp
 * @brief      图片消息气泡，继承 BubbleBase
 *
 * @author     KBchulan
 * @date       2026/01/02
 * @history
 ******************************************************************************/

#ifndef PICTUREBUBBLE_HPP
#define PICTUREBUBBLE_HPP

#include "bubblebase.hpp"

class QLabel;
class QPaintEvent;
class PictureBubble : public BubbleBase
{
  Q_OBJECT

public:
  explicit PictureBubble(MsgDirection direction, const QString& image_path = QString(), QWidget* parent = nullptr);
  explicit PictureBubble(MsgDirection direction, const QPixmap& pixmap, QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  void initUI();
  void updatePixmap(const QPixmap& pixmap);
  void setImage(const QString& image_path);
  void setImage(const QPixmap& pixmap);

  // 图片最大尺寸
  static constexpr int kMaxImageWidth = 450;
  static constexpr int kMaxImageHeight = 400;

  QLabel* _image_label;
};

#endif  // PICTUREBUBBLE_HPP
