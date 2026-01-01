/******************************************************************************
 *
 * @file       timerbutton.hpp
 * @brief      实现一个带倒计时功能的按钮
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef TIMERBUTTON_HPP
#define TIMERBUTTON_HPP

#include <QObject>
#include <QPushButton>
#include <QTimer>

class TimerButton final : public QPushButton
{
  Q_OBJECT

  static constexpr int kDefaultCountdown = 60;

public:
  TimerButton(QWidget* parent = nullptr);
  ~TimerButton() = default;

  void Reset();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  void reset_timer();

private:
  int _counter;
  QTimer* _timer;
};

#endif  // TIMERBUTTON_HPP
