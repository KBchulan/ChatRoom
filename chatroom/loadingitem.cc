#include "loadingitem.hpp"

#include "ui_loadingitem.h"

LoadingItem::LoadingItem(const QString& tip, QWidget* parent) : QWidget(parent), ui(new Ui::LoadingItem)
{
  ui->setupUi(this);

  ui->tip_label->setText(tip);

  _movie = new QMovie(":/icons/loading.gif");
  _movie->setScaledSize(QSize(50, 50));

  ui->loading_label->setMovie(_movie);
  _movie->start();
}

LoadingItem::~LoadingItem()
{
  _movie->stop();
  delete _movie;
  delete ui;
}
