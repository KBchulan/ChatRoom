#include "friendapplypage.hpp"

#include "ui_friendapplypage.h"

FriendApplyPage::FriendApplyPage(QWidget* parent) : QWidget(parent), ui(new Ui::FriendApplyPage)
{
  ui->setupUi(this);

  // 初始化ui
  initUI();
}

FriendApplyPage::~FriendApplyPage()
{
  delete ui;
}

void FriendApplyPage::initUI()
{
  this->ui->tip_label->setText("新的朋友");
}
