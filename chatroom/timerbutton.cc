#include "timerbutton.hpp"

#include <QMouseEvent>

TimerButton::TimerButton(QWidget *parent) : QPushButton(parent), _counter(kDefaultCountdown)
{
  _timer = new QTimer(this);

  connect(_timer, &QTimer::timeout, this, [this]() -> void
  {
    --_counter;
    if (_counter <= 0)
    {
      reset_timer();
      return;
    }
    this->setText(QString::number(_counter));
  });
}

void TimerButton::Reset()
{
  reset_timer();
}

void TimerButton::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    this->setEnabled(false);
    this->setText(QString::number(_counter));
    _timer->start(1000);
    emit clicked();
  }
}

void TimerButton::reset_timer()
{
  _timer->stop();
  _counter = kDefaultCountdown;
  this->setText("获取");
  this->setEnabled(true);
}
