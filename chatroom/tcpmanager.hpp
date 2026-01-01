/******************************************************************************
 *
 * @file       tcpmanager.hpp
 * @brief      全局的 TCP 连接管理器, 负责与服务器的通信
 *
 * @author     KBchulan
 * @date       2026/01/01
 * @history
 ******************************************************************************/

#ifndef TCPMANAGER_HPP
#define TCPMANAGER_HPP

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <cstdint>
#include <functional>

#include "global.hpp"
#include "serverinfo.hpp"

class TcpManager : public QObject
{
  Q_OBJECT

public:
  static TcpManager& GetInstance();

  // 断开连接
  void Disconnect();

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

  QMap<ReqID, std::function<void(ReqID rid, int len, const QByteArray& data)>> _handlers;

public slots:
  void SlotTcpConnect(const ServerInfo&);
  void SlotSendData(ReqID reqId, const QString& data);

signals:
  void sig_conn_finish(bool success);
  void sig_send_data(ReqID reqId, QString data);
  void sig_switch_chat_dialog();
  void sig_login_failed(int);
};

#endif  // TCPMANAGER_HPP
