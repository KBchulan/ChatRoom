#ifndef CHATDIALOG_HPP
#define CHATDIALOG_HPP

#include <QDialog>

namespace Ui
{
class ChatDialog;
}

class ChatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ChatDialog(QWidget* parent = nullptr);
  ~ChatDialog();

private:
  Ui::ChatDialog* ui;
};

#endif  // CHATDIALOG_HPP
