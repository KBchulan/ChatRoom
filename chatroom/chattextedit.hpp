/******************************************************************************
 *
 * @file       chattextedit.hpp
 * @brief      聊天输入框，自定义QTextEdit，支持富文本编辑和图片拖放以及复制粘贴
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

class QDragEnterEvent;
class QDropEvent;
class QMimeData;
class ChatTextEdit : public QTextEdit
{
  Q_OBJECT

public:
  explicit ChatTextEdit(QWidget* parent = nullptr);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;
  void insertFromMimeData(const QMimeData* source) override;

  // 插入图片
  void insertImage(const QString& image_path);
  void insertImage(const QImage& image);

private:
  // 缩略图最大尺寸
  static constexpr int kMaxThumbnailWidth = 350;
  static constexpr int kMaxThumbnailHeight = 300;

  [[nodiscard]] static QImage scaledThumbnail(const QImage& image);
};

#endif  // CHATTEXTEDIT_HPP
