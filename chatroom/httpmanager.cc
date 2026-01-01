#include "httpmanager.hpp"

#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "Defer.hpp"

HttpManager::HttpManager()
{
  connect(this, &HttpManager::sig_post_http_finish, this, &HttpManager::slot_post_http_finish);
}

HttpManager::~HttpManager()
{
}

void HttpManager::PostHttpReq(const QUrl& url, const QJsonObject& json, ReqID rid, Module mod)
{
  QByteArray data = QJsonDocument(json).toJson();

  QNetworkRequest request{url};
  request.setRawHeader("Connection", "keep-alive");
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QNetworkReply* reply = _manager.post(request, data);

  connect(reply, &QNetworkReply::finished,
          [this, reply, rid, mod]() -> void
          {
            // 回收 reply
            defer(reply->deleteLater());

            QString res = reply->readAll();

            // 存在错误
            if (reply->error() != QNetworkReply::NoError)
            {
              qDebug() << "Http post error, module is: " << static_cast<int>(mod)
                       << "req id is: " << static_cast<int>(rid) << "err msg is: " << reply->errorString() << '\n';

              emit sig_post_http_finish(res, ErrorCode::NETWORK_ERROR, rid, mod);
              return;
            }

            // 没有错误
            emit sig_post_http_finish(res, ErrorCode::SUCCESS, rid, mod);
          });
}

void HttpManager::slot_post_http_finish(const QString& str, ErrorCode err, ReqID rid, Module mod)
{
  if (mod == Module::REGISTER)
  {
    emit sig_register_mod_finish(str, err, rid);
  }
  else if (mod == Module::RESET)
  {
    emit sig_reset_mod_finish(str, err, rid);
  }
  else if (mod == Module::LOGIN)
  {
    emit sig_login_mod_finish(str, err, rid);
  }
}
