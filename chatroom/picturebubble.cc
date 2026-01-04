#include "picturebubble.hpp"

#include <QHBoxLayout>
#include <QLabel>

PictureBubble::PictureBubble(MsgDirection direction, const QString& image_path, QWidget* parent)
    : BubbleBase(direction, parent)
{
  initUI();

  if (!image_path.isEmpty())
  {
    setImage(image_path);
  }
}

PictureBubble::PictureBubble(MsgDirection direction, const QPixmap& pixmap, QWidget* parent)
    : BubbleBase(direction, parent)
{
  initUI();

  if (!pixmap.isNull())
  {
    setImage(pixmap);
  }
}

void PictureBubble::initUI()
{
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  _image_label = new QLabel(this);
  _image_label->setObjectName("bubble_image");
  _image_label->setAlignment(Qt::AlignCenter);
  layout->addWidget(_image_label);
}

void PictureBubble::paintEvent([[maybe_unused]] QPaintEvent* event)
{
  // 图片消息不绘制气泡背景, 这里覆写一下，不让基类绘制
}

void PictureBubble::setImage(const QString& image_path)
{
  QPixmap pixmap(image_path);
  if (!pixmap.isNull())
  {
    updatePixmap(pixmap);
  }
}

void PictureBubble::setImage(const QPixmap& pixmap)
{
  if (!pixmap.isNull())
  {
    updatePixmap(pixmap);
  }
}

void PictureBubble::updatePixmap(const QPixmap& pixmap)
{
  QPixmap scaled = pixmap;

  // 如果图片超过最大尺寸，按比例缩放
  if (pixmap.width() > kMaxImageWidth || pixmap.height() > kMaxImageHeight)
  {
    scaled = pixmap.scaled(kMaxImageWidth, kMaxImageHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  _image_label->setPixmap(scaled);
  adjustSize();
}
