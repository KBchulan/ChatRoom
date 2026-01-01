#include "chatpage.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>

#include "ui_chatpage.h"

ChatPage::ChatPage(QWidget* parent) : QWidget(parent), ui(new Ui::ChatPage)
{
  ui->setupUi(this);

  ui->emoji_label->setAttribute(Qt::WA_Hover, true);
  ui->file_label->setAttribute(Qt::WA_Hover, true);

  addTestData();
}

ChatPage::~ChatPage()
{
  delete ui;
}

void ChatPage::addTestData()
{
  const QStringList msgs = {"你好，最近怎么样？", "挺好的，你呢？",     "我也不错，周末有空吗？", "有空啊，想干什么？",
                            "一起去爬山吧！",     "好主意，几点出发？", "早上8点怎么样？",        "没问题，到时候见！",
                            "记得带水和零食",     "好的，我会准备的",   "那就这么说定了",         "嗯嗯，期待周末！"};

  for (const auto& msg : msgs)
  {
    auto* item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 40));

    // TODO: 后续替换为 ChatMsgItem
    auto* widget = new QWidget();
    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(10, 5, 10, 5);

    auto* label = new QLabel(msg);
    layout->addWidget(label);

    ui->chat_msg_list->addItem(item);
    ui->chat_msg_list->setItemWidget(item, widget);
  }
}
