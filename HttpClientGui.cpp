//#include <QString>
#include "HttpClientGui.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>

HttpClient::HttpClient(QObject * pobj) : QObject(pobj) {
  const QString serverLink = "http://s5.goodfon.ru/"; //"http://cbr.ru";
  // /scripts/XML_daily.asp
  pNetManager = new QNetworkAccessManager(this);
  connect(pNetManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(slotFinished(QNetworkReply *)));
}

void HttpClient::slotFinished(QNetworkReply *) {

}
