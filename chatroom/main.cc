#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QIcon>
#include <QSettings>

#include "global.hpp"
#include "mainwindow.hpp"

void loadResources(QApplication& app)
{
  // config.ini
  QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
  QSettings settings(configPath, QSettings::IniFormat);

  QString host = settings.value("ChatRoomBackend/host").toString();
  int port = settings.value("ChatRoomBackend/port").toInt();
  CHATROOM_API_BASE_URL = QString("http://%1:%2/api/v1").arg(host).arg(port);

  // 项目 icon
  QApplication::setWindowIcon(QIcon(":/chatroom.png"));

  // 界面样式
  QString styleSheet;

  QFile main_qss(":/style/main.qss");
  if (main_qss.open(QFile::ReadOnly))
  {
    styleSheet += QLatin1String(main_qss.readAll());
    main_qss.close();
  }

  QFile login_qss(":/style/login_window.qss");
  if (login_qss.open(QFile::ReadOnly))
  {
    styleSheet += QLatin1String(login_qss.readAll());
    login_qss.close();
  }

  QFile register_qss(":/style/register_window.qss");
  if (register_qss.open(QFile::ReadOnly))
  {
    styleSheet += QLatin1String(register_qss.readAll());
    register_qss.close();
  }

  QFile reset_qss(":/style/reset_pass_window.qss");
  if (reset_qss.open(QFile::ReadOnly))
  {
    styleSheet += QLatin1String(reset_qss.readAll());
    reset_qss.close();
  }

  QFile chat_qss(":/style/chat_window.qss");
  if (chat_qss.open(QFile::ReadOnly))
  {
    styleSheet += QLatin1String(chat_qss.readAll());
    chat_qss.close();
  }

  app.setStyleSheet(styleSheet);
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  // 加载资源文件
  loadResources(app);

  // 设置主窗口
  MainWindow main_window;
  main_window.show();

  return QApplication::exec();
}
