/******************************************************************************
 *
 * @file       httpmanager.hpp
 * @brief      全局的 http 管理器
 *
 * @author     KBchulan
 * @date       2025/12/01
 * @history
 *****************************************************************************/

#ifndef HTTPMANAGER_HPP
#define HTTPMANAGER_HPP

#include <QJsonObject>
#include <QUrl>
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

#include "global.hpp"

class HttpManager : public QObject
{
  Q_OBJECT

public:
  HttpManager(HttpManager&&) = delete;
  HttpManager& operator=(HttpManager&&) = delete;

  static HttpManager& GetInstance()
  {
    static HttpManager instance;
    return instance;
  }

  void PostHttpReq(const QUrl& url, const QJsonObject& json, ReqID id, Module mod);

private:
  HttpManager();

  ~HttpManager();

private:
  QNetworkAccessManager _manager;

private slots:
  void slot_post_http_finish(QString str, ErrorCode err, ReqID id, Module mod);

signals:
  void sig_post_http_finish(QString str, ErrorCode err, ReqID id, Module mod);
  void sig_register_mod_finish(QString str, ErrorCode err, ReqID id);
};

#endif  // HTTPMANAGER_HPP
