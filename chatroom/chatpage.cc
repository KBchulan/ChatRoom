#include "chatpage.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextFragment>
#include <QTimer>

#include "chatmsgitem.hpp"
#include "chatmsglist.hpp"
#include "global.hpp"
#include "loadingitem.hpp"
#include "picturebubble.hpp"
#include "textbubble.hpp"
#include "ui_chatpage.h"

ChatPage::ChatPage(QWidget* parent) : QWidget(parent), ui(new Ui::ChatPage)
{
  ui->setupUi(this);

  ui->emoji_label->setAttribute(Qt::WA_Hover, true);
  ui->file_label->setAttribute(Qt::WA_Hover, true);

  connect(ui->chat_msg_list, &ChatMsgList::sig_load_more_msg, this, &ChatPage::slot_load_more_msg);
}

ChatPage::~ChatPage()
{
  delete ui;
}

void ChatPage::on_send_button_clicked()
{
  // 获取输入框的内容
  QTextDocument* doc = ui->chat_edit->document();
  if (doc->isEmpty())
  {
    return;
  }

  // 用于累积连续的文本
  QString accumulated_text;

  // 发送文本消息的辅助函数
  auto sendTextMsg = [this](const QString& text)
  {
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty())
    {
      return;
    }

    auto* msg_item = new ChatMsgItem(MsgDirection::Sent);
    msg_item->setAvatar(QPixmap(":/avatar/head_1.jpg"));
    msg_item->setNickname("KBchulan", true);

    auto* bubble = new TextBubble(MsgDirection::Sent, trimmed);
    msg_item->setBubble(bubble);

    ui->chat_msg_list->appendChatItem(msg_item);
  };

  // 发送图片消息的辅助函数
  auto sendImageMsg = [this](const QImage& image)
  {
    auto* msg_item = new ChatMsgItem(MsgDirection::Sent);
    msg_item->setAvatar(QPixmap(":/avatar/head_1.jpg"));
    msg_item->setNickname("KBchulan", true);

    auto* bubble = new PictureBubble(MsgDirection::Sent, QPixmap::fromImage(image));
    msg_item->setBubble(bubble);

    ui->chat_msg_list->appendChatItem(msg_item);
  };

  // 遍历文档，提取文本和图片
  for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next())
  {
    for (auto it = block.begin(); !it.atEnd(); ++it)
    {
      QTextFragment fragment = it.fragment();
      if (!fragment.isValid())
      {
        continue;
      }

      QTextCharFormat format = fragment.charFormat();

      if (format.isImageFormat())
      {
        // 遇到图片时，先发送累积的文本
        sendTextMsg(accumulated_text);
        accumulated_text.clear();

        // 发送图片
        QTextImageFormat img_format = format.toImageFormat();
        auto image = doc->resource(QTextDocument::ImageResource, QUrl(img_format.name())).value<QImage>();
        if (!image.isNull())
        {
          sendImageMsg(image);
        }
      }
      else
      {
        // 累积文本
        accumulated_text += fragment.text();
      }
    }

    if (block.next().isValid())
    {
      accumulated_text += '\n';
    }
  }

  sendTextMsg(accumulated_text);
  ui->chat_edit->clear();
}

void ChatPage::on_receive_button_clicked()
{
  // 测试用：模拟接收消息
  auto* msg_item = new ChatMsgItem(MsgDirection::Received);
  msg_item->setAvatar(QPixmap(":/avatar/head_2.jpg"));
  msg_item->setNickname("测试用户", true);

  auto* bubble = new TextBubble(MsgDirection::Received, "这是一条测试的接收消息");
  msg_item->setBubble(bubble);

  ui->chat_msg_list->appendChatItem(msg_item);
}

void ChatPage::slot_load_more_msg()
{
  if (_loading)
  {
    return;
  }

  _loading = true;
  showLoading();

  // TODO: 模拟网络延迟，后续替换为真实请求
  QTimer::singleShot(1000, this,
                     [this]()
                     {
                       hideLoading();
                       addTestHistoryData();
                     });
}

void ChatPage::showLoading()
{
  _loading_item = new QListWidgetItem();
  _loading_item->setSizeHint(QSize(0, UIConstants::LoadingItemHeight));

  auto* widget = new LoadingItem("加载历史消息...");
  ui->chat_msg_list->insertItem(0, _loading_item);
  ui->chat_msg_list->setItemWidget(_loading_item, widget);
}

void ChatPage::hideLoading()
{
  if (_loading_item != nullptr)
  {
    delete ui->chat_msg_list->itemWidget(_loading_item);
    delete ui->chat_msg_list->takeItem(ui->chat_msg_list->row(_loading_item));
    _loading_item = nullptr;
  }
  _loading = false;
}

void ChatPage::addTestHistoryData()
{
  const QStringList msgs = {"这是历史消息 1", "这是历史消息 2", "这是历史消息 3", "这是历史消息 4", "这是历史消息 5",
                            "周末有空吗？",   "好的，收到",     "明天见～",       "哈哈哈哈"};

  // 记录插入前的滚动位置
  auto* scrollbar = ui->chat_msg_list->verticalScrollBar();
  int old_scroll = scrollbar->value();
  int old_max = scrollbar->maximum();

  for (int i = 0; i < 9; ++i)
  {
    auto* msg_item = new ChatMsgItem(i % 2 == 0 ? MsgDirection::Received : MsgDirection::Sent);
    msg_item->setAvatar(QPixmap(i % 2 == 0 ? ":/avatar/head_2.jpg" : ":/avatar/head_1.jpg"));
    msg_item->setNickname(i % 2 == 0 ? "历史用户" : "KBchulan", true);

    auto* bubble = new TextBubble(i % 2 == 0 ? MsgDirection::Received : MsgDirection::Sent, msgs[i]);
    msg_item->setBubble(bubble);

    // 在顶部插入
    auto* list_item = new QListWidgetItem();
    list_item->setSizeHint(msg_item->sizeHint());
    ui->chat_msg_list->insertItem(0, list_item);
    ui->chat_msg_list->setItemWidget(list_item, msg_item);
  }

  // 补偿滚动位置，保持用户阅读位置
  int new_max = scrollbar->maximum();
  scrollbar->setValue(old_scroll + (new_max - old_max));
}
