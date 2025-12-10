#ifndef TIMERBUTTON_HPP
#define TIMERBUTTON_HPP

#include <QTimer>
#include <QPushButton>
#include <QObject>

class TimerButton final : public QPushButton
{
  Q_OBJECT

  static constexpr int kDefaultCountdown = 60;

public:
  TimerButton(QWidget *parent = nullptr);
  ~TimerButton() = default;

  void Reset();

protected:
  void mouseReleaseEvent(QMouseEvent *e) override;

private:
  void reset_timer();

private:
  int _counter;
  QTimer* _timer;
};

#endif  // TIMERBUTTON_HPP
