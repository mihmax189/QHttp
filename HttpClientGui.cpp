//#include <QString>
#include "HttpClientGui.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>

HttpClient::HttpClient(QObject * pobj) : QObject(pobj) {
  const QString serverLink = "http://s5.goodfon.ru/"; //"http://cbr.ru";
  // /scripts/XML_daily.asp
  pNetManager = new QNetworkAccessManager(this);
  // сигнал finished() отправляет по окончании загрузки
  connect(pNetManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(slotFinished(QNetworkReply *)));
}

void HttpClient::slotFinished(QNetworkReply *) {
  // объект QNetworkReply соответствует текущей проверенной операции
}

void HttpClient::download(const QUrl & url) {
  QNetworkRequest request(url);                       // входной объект запроса для pReply
  /*QNetworkReply * pReply =*/ pNetManager->get(request); // выполнение запроса
  //connect(pReply, SIGNAL())
}
