#include "httpmanager.hpp"
#include "Defer.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QByteArray>
#include <QDebug>

HttpManager::HttpManager()
{
  connect(this, &HttpManager::sig_post_http_finish, this, &HttpManager::slot_post_http_finish);
}

HttpManager::~HttpManager()
{

}

void HttpManager::PostHttpReq(const QUrl& url, const QJsonObject& json, ReqID id, Module mod)
{
  QByteArray data = QJsonDocument(json).toJson();

  QNetworkRequest request{url};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QNetworkReply* reply = _manager.post(request, data);

  connect(reply, &QNetworkReply::finished, [this, reply, id, mod]() -> void
  {
    // 回收 reply
    defer(reply->deleteLater());

    QString res = reply->readAll();

    // 存在错误
    if (reply->error() != QNetworkReply::NoError)
    {
      qDebug() << "Http post error, module is: " << static_cast<int>(mod)
               << "req id is: " << static_cast<int>(id)
               << "err msg is: " << reply->errorString() << '\n';

      emit sig_post_http_finish(res, ErrorCode::NETWORK_ERROR, id, mod);
      return;
    }

    // 没有错误
    emit sig_post_http_finish(res, ErrorCode::SUCCESS, id, mod);
  });
}

void HttpManager::slot_post_http_finish(QString str, ErrorCode err, ReqID id, Module mod)
{
  if (mod == Module::REGISTER)
  {
    emit sig_register_mod_finish(str, err, id);
  }
  else if (mod == Module::RESET)
  {
    emit sig_reset_mod_finish(str, err, id);
  }
}
