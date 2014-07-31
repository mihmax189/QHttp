#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <QHttp>
#include <QObject>
#include <QUrl>
#include <QWidget>
#include <QtXml>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QProgressBar;
class QLineEdit;
class QLabel;
class QPushButton;

class HttpClient : public QObject {
Q_OBJECT;
private:
  QHttp httpServer;
  QNetworkAccessManager * pNetManager;

public:
  HttpClient(QObject * pwgt = 0);
  void download(const QUrl &);                 // загрузка ресурса, заданного в параметр QUrl

signals:
  void done(const QUrl &, const QByteArray &); // сигнал о завершении загрузки
  void error();                                // сигнал о ошибке загрузки
  void downloadProgress(qint64, qint64);

private slots:
  void slotFinished(QNetworkReply *);          // вызывается по завершении операции загрузки
};


// реализация GUI
class HttpClientGui : public QWidget {
  Q_OBJECT
private:
  HttpClient * pClient;
  QProgressBar * pProgBar;
  QLineEdit * pRateDollarLineEdit;
  QLineEdit * pRateEuroLineEdit;
  QLabel * pHeaderRateDollar;
  QLabel * pHeaderRateEuro;
  QPushButton * pGoButton;
  QString strDownloadLink;
  //void showPic(const QString &);

public:
  HttpClientGui(QWidget * pwgt = 0);

private slots:
  void slotGo();                                   // осуществляет испольнение загрузки ресурса
  void slotError();                                // вызывает при возникновении ошибки
  void slotDownloadProgress(qint64, qint64);       // отображает процесс загрузки
  //void sloteDone(const QUrl &, const QByteArray);  // выполняет конечные действия по окончании
                                                     // загрузки
};

#endif // HTTP_CLIENT_H_
