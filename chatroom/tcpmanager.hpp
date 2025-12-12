#ifndef TCPMANAGER_HPP
#define TCPMANAGER_HPP

#include <QObject>
#include <QByteArray>
#include <cstdint>
#include <functional>
#include <QMap>
#include <QTcpSocket>
#include <QString>
#include <QtTypes>

#include "global.hpp"
#include "serverinfo.hpp"

class TcpManager : public QObject
{
  Q_OBJECT

public:
  static TcpManager& GetInstance();

private:
  TcpManager();
  ~TcpManager();

  void init_handlers();

  QTcpSocket _socket;
  QString _host;
  std::uint16_t _port;

  bool _recv_pending;

  // TLV 格式
  quint16 _message_id;
  quint16 _message_len;
  QByteArray _buffer;

  QMap<ReqID, std::function<void(ReqID id, int len, QByteArray data)>> _handlers;

public slots:
  void SlotTcpConnect(const ServerInfo&);
  void SlotSendData(ReqID reqId, QString data);

signals:
  void sig_conn_finish(bool success);
  void sig_send_data(ReqID reqId, QString data);
  void sig_switch_chat_dialog();
  void sig_login_failed(int);
};

#endif  // TCPMANAGER_HPP
