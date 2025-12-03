#include <QApplication>
#include <QIcon>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>

#include "global.hpp"
#include "mainwindow.hpp"

void loadResources(QApplication &app)
{
  // config.ini
  QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
  QSettings settings(configPath, QSettings::IniFormat);

  QString host = settings.value("GateWay/host").toString();
  int port = settings.value("GateWay/port").toInt();
  GateWayUrl = QString("http://%1:%2").arg(host).arg(port);

  // 项目 icon
  app.setWindowIcon(QIcon(":/chatroom.png"));

  // 界面样式
  QString styleSheet;

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

  app.setStyleSheet(styleSheet);
}

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  // 加载资源文件
  loadResources(a);

  // 设置主窗口
  MainWindow w;
  w.show();

  return a.exec();
}
