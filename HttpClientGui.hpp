#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

//#include <QGui>
#include <QHttp>
#include <QObject>
#include <QUrl>
#include <QWidget>
#include <QtXml>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class HttpClient : public QObject {
Q_OBJECT;
private:
  QHttp httpServer;
  QNetworkAccessManager * pNetManager;
  void download(const QUrl &);                 // загрузка ресурса, заданного в параметр QUrl
public:
  HttpClient(QObject * pwgt = 0);

signals:
  void done(const QUrl &, const QByteArray &); // сигнал о завершении загрузки
  void error();                                // сигнал о ошибке загрузки

private slots:
  void slotFinished(QNetworkReply *);          // вызывается в конце загрузки
};

#endif // HTTP_CLIENT_H_
