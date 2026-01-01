#include "chatmsgitem.hpp"

#include "ui_chatmsgitem.h"

ChatMsgItem::ChatMsgItem(QWidget* parent) : QWidget(parent), ui(new Ui::ChatMsgItem)
{
  ui->setupUi(this);
}

ChatMsgItem::~ChatMsgItem()
{
  delete ui;
}
