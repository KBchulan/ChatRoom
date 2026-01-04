#include "chattextedit.hpp"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImageReader>
#include <QMimeData>
#include <QScrollBar>
#include <QStyle>
#include <QTextCursor>

ChatTextEdit::ChatTextEdit(QWidget* parent) : QTextEdit(parent)
{
  // 始终显示滚动条，通过样式控制透明度来避免文字重排
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // 默认隐藏滚动条
  verticalScrollBar()->setProperty("hover", false);
  verticalScrollBar()->style()->unpolish(verticalScrollBar());
  verticalScrollBar()->style()->polish(verticalScrollBar());

  // 启用拖放
  setAcceptDrops(true);
}

void ChatTextEdit::enterEvent(QEnterEvent* event)
{
  verticalScrollBar()->setProperty("hover", true);
  verticalScrollBar()->style()->unpolish(verticalScrollBar());
  verticalScrollBar()->style()->polish(verticalScrollBar());
  QTextEdit::enterEvent(event);
}

void ChatTextEdit::leaveEvent(QEvent* event)
{
  verticalScrollBar()->setProperty("hover", false);
  verticalScrollBar()->style()->unpolish(verticalScrollBar());
  verticalScrollBar()->style()->polish(verticalScrollBar());
  QTextEdit::leaveEvent(event);
}

void ChatTextEdit::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasImage() || event->mimeData()->hasUrls())
  {
    // 检查 URL 是否为图片
    bool has_image = event->mimeData()->hasImage();
    if (!has_image && event->mimeData()->hasUrls())
    {
      for (const auto& url : event->mimeData()->urls())
      {
        QString suffix = url.toLocalFile().section('.', -1).toLower();
        if (QImageReader::supportedImageFormats().contains(suffix.toUtf8()))
        {
          has_image = true;
          break;
        }
      }
    }

    if (has_image)
    {
      event->acceptProposedAction();
      return;
    }
  }

  QTextEdit::dragEnterEvent(event);
}

void ChatTextEdit::dropEvent(QDropEvent* event)
{
  const QMimeData* mime = event->mimeData();

  // 处理直接拖入的图片
  if (mime->hasImage())
  {
    auto image = qvariant_cast<QImage>(mime->imageData());
    insertImage(image);
    event->acceptProposedAction();
    return;
  }

  // 处理文件 URL
  if (mime->hasUrls())
  {
    for (const auto& url : mime->urls())
    {
      QString file_path = url.toLocalFile();
      QString suffix = file_path.section('.', -1).toLower();

      if (QImageReader::supportedImageFormats().contains(suffix.toUtf8()))
      {
        insertImage(file_path);
        event->acceptProposedAction();
        return;
      }
    }
  }

  QTextEdit::dropEvent(event);
}

void ChatTextEdit::insertFromMimeData(const QMimeData* source)
{
  // 1. 图片文件 URL
  if (source->hasUrls())
  {
    for (const auto& url : source->urls())
    {
      QString suffix = url.toLocalFile().section('.', -1).toLower();
      if (QImageReader::supportedImageFormats().contains(suffix.toUtf8()))
      {
        insertImage(url.toLocalFile());
        return;
      }
    }
  }

  // 2. 只有图片没有文本
  if (source->hasImage() && !source->hasText())
  {
    auto image = qvariant_cast<QImage>(source->imageData());
    if (!image.isNull())
    {
      insertImage(image);
      return;
    }
  }

  // 3. 单纯的文本情况
  if (source->hasText())
  {
    QString text = source->text();
    if (!text.isEmpty())
    {
      insertPlainText(text);
      return;
    }
  }
}

void ChatTextEdit::insertImage(const QString& image_path)
{
  QImage image(image_path);
  if (!image.isNull())
  {
    insertImage(image);
  }
}

void ChatTextEdit::insertImage(const QImage& image)
{
  if (image.isNull())
  {
    return;
  }

  QImage thumbnail = scaledThumbnail(image);

  QTextCursor cursor = textCursor();

  if (!cursor.atBlockStart())
  {
    cursor.insertText("\n");
  }
  cursor.insertImage(thumbnail);
  cursor.insertText("\n");

  setTextCursor(cursor);
}

QImage ChatTextEdit::scaledThumbnail(const QImage& image)
{
  if (image.width() <= kMaxThumbnailWidth && image.height() <= kMaxThumbnailHeight)
  {
    return image;
  }

  return image.scaled(kMaxThumbnailWidth, kMaxThumbnailHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
