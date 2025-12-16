#include "tcpmanager.hpp"
#include "userinfo.hpp"

#include <QtEndian>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAbstractSocket>

TcpManager &TcpManager::GetInstance()
{
  static TcpManager instance;
  return instance;
}

TcpManager::TcpManager() : _host(""), _port(0), _recv_pending(false), _message_id(0), _message_len(0)
{
  connect(&_socket, &QTcpSocket::connected, this, [this]() -> void
  {
    qDebug() << "Connected to server!";
    emit sig_conn_finish(true);
  });

  connect(&_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err)
  {
    qDebug() << "Socket error:" << _socket.errorString();

    if (err == QTcpSocket::ConnectionRefusedError ||
        err == QTcpSocket::HostNotFoundError ||
        err == QTcpSocket::SocketTimeoutError)
    {
      emit sig_conn_finish(false);
    }
  });

  connect(&_socket, &QTcpSocket::disconnected, this, [this]()
  {
    qDebug() << "Disconnected from server.";
  });

  connect(&_socket, &QTcpSocket::readyRead, this, [this]() -> void
  {
    // 读取所有数据
    _buffer.append(_socket.readAll());

    while (true)
    {
      // 1. 解析头部
      if (!_recv_pending)
      {
        if (_buffer.size() < 2 * sizeof(quint16)) return;

        _message_id = qFromBigEndian<quint16>(_buffer.constData());
        _message_len = qFromBigEndian<quint16>(_buffer.constData() + 2);
        _buffer.remove(0, 4);

        qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
      }

      // 2. 检查消息体是否完整
      if (_buffer.size() < _message_len)
      {
        _recv_pending = true;
        return;
      }

      // 3. 直接提取消息体
      _recv_pending = false;
      QByteArray body = _buffer.left(_message_len);
      _buffer.remove(0, _message_len);

      qDebug() << "Receive body:" << body;

      auto reqID = static_cast<ReqID>(_message_id);
      _handlers[reqID](reqID, _message_len, body);
    }

  });

  connect(this, &TcpManager::sig_send_data, this, &TcpManager::SlotSendData);

  // 注册回调
  init_handlers();
}

TcpManager::~TcpManager()
{
  _socket.disconnectFromHost();
}

void TcpManager::init_handlers()
{
  _handlers.insert(ReqID::ID_LOGIN_CHAT_RESPONSE, [this](ReqID id, int len, QByteArray data) -> void
  {
    // 解析数据
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (jsonDoc.isNull())
    {
      qDebug() << "Receive from id: " << static_cast<int>(id) << ", len: " << len << ", data: " << data << '\n';
      return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 解析响应数据并存下
    auto code = jsonObj["code"].toInt();
    auto message = jsonObj["message"].toString();

    if (code != 0)
    {
      qDebug() << "login failed, msg is: " << message << '\n';
      emit sig_login_failed(code);
      return;
    }

    if (jsonObj.contains("data") && jsonObj["data"].isObject()) {
      const QJsonObject data = jsonObj["data"].toObject();
      UserInfo::GetInstance().SetNickname(data["nickname"].toString());
      UserInfo::GetInstance().SetAvatar(data["avatar"].toString());
      UserInfo::GetInstance().SetEmail(data["email"].toString());
    }
    emit sig_switch_chat_dialog();
  });
}

void TcpManager::SlotTcpConnect(const ServerInfo &si)
{
  _host = si.GetHost();
  _port = si.GetPort();
  _socket.connectToHost(_host, _port);
}

void TcpManager::SlotSendData(ReqID reqId, QString data)
{
  quint16 id = static_cast<quint16>(reqId);
  QByteArray body = data.toUtf8();
  quint16 len = static_cast<quint16>(body.size());

  QByteArray packet;
  packet.reserve(4 + body.size());

  // 写入大端序数据
  packet.append(static_cast<char>(id >> 8));
  packet.append(static_cast<char>(id & 0xFF));
  packet.append(static_cast<char>(len >> 8));
  packet.append(static_cast<char>(len & 0xFF));
  packet.append(body);

  _socket.write(packet);
}
