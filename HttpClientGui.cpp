//#include <QString>
#include "HttpClientGui.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>

HttpClient::HttpClient(QObject * pobj) : QObject(pobj) {
  const QString serverLink = "http://s5.goodfon.ru/"; //"http://cbr.ru";
  // /scripts/XML_daily.asp
  pNetManager = new QNetworkAccessManager(this);
  // сигнал finished() отправляет по окончании загрузки
  connect(pNetManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(slotFinished(QNetworkReply *)));
}

void HttpClient::download(const QUrl & url) {
  QNetworkRequest request(url);                           // входной объект запроса для pReply
  QNetworkReply * pReply = pNetManager->get(request); // выполнение запроса
  connect(pReply, SIGNAL(downloadProgress(qint64, qint64)),
          this, SIGNAL(downloadProgress(qint64, qint64)));
}


void HttpClient::slotFinished(QNetworkReply * pReply) {
  // объект QNetworkReply соответствует текущей проверенной операции
  if (pReply->error() != QNetworkReply::NoError)
    emit error();
  else
    emit done(pReply->url(), pReply->readAll());          // передать полученные данные дальше
  pReply->deleteLater();                                  // удаление объекта QNetworkReply
}

// HttpClientGui methods
HttpClientGui::HttpClientGui(QWidget * pwgt) : QWidget(pwgt) {
  pClient = new HttpClient(this);
  pProgBar = new QProgressBar;
  pHeaderRateDollar = new QLabel("Rate of Dolllar: ");
  pHeaderRateEuro = new QLabel("Rate of Euro: ");
  pRateDollarLineEdit = new QLineEdit;
  pRateDollarLineEdit->setReadOnly(true);
  pRateEuroLineEdit = new QLineEdit;
  pRateEuroLineEdit->setReadOnly(true);
  pGoButton = new QPushButton("Go");
  strDownloadLink = "http://cbr.ru/scripts/XML_daily.asp";

  //connect(pGoButton, SIGNAL(clicked()), SLOT(slotGo()));
  //connect(pClient, SIGNAL(downloadProgress(qint64, qint64)),
  //        this, SLOT(slotDownloadProgress(qint64, qint64)));
  //connect(pClient, SIGNAL(done(const QUrl &, const QByteArray &)),
  //        this, SLOT(slotDone(const QUrl &, const QByteArray &)));

  QGridLayout * pMainLayout = new QGridLayout;
  pMainLayout->addWidget(pHeaderRateDollar, 0, 0);
  pMainLayout->addWidget(pRateDollarLineEdit, 0, 1);
  pMainLayout->addWidget(pHeaderRateEuro, 1, 0);
  pMainLayout->addWidget(pRateEuroLineEdit, 1, 1);
  pMainLayout->addWidget(pProgBar, 2, 0);
  pMainLayout->addWidget(pGoButton, 3, 0);

  setLayout(pMainLayout);
}

// запуск процесса загрузки
void HttpClientGui::slotGo() {
  pClient->download(QUrl(strDownloadLink));
}

void HttpClientGui::slotDownloadProgress(qint64 nReceived, qint64 nTotal) {
  // nTotal -- размер загружаемого файла
  if (nTotal <= 0) {
    slotError();
    return;
  }
  pProgBar->setValue(100 * nReceived / nTotal);
}

void HttpClientGui::slotError() {

}
