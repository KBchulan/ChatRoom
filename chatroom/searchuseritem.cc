#include "searchuseritem.hpp"

#include "ui_searchuseritem.h"

SearchUserItem::SearchUserItem(QWidget* parent) : QWidget(parent), ui(new Ui::SearchUserItem)
{
  ui->setupUi(this);
}

SearchUserItem::~SearchUserItem()
{
  delete ui;
}
