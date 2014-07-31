#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

//#include <QGui>
#include <QHttp>
#include <QObject>
#include <QUrl>
#include <QWidget>
#include <QtXml>

class HttpClientGui : public QWidget {
Q_OBJECT
private:
  QHttp httpServer;

public:
  HttpClientGui(QWidget * pwgt = 0); //(QObject * pobj = 0);
};

#endif // HTTP_CLIENT_H_
