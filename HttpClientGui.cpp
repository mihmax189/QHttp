//#include <QString>
#include "HttpClientGui.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

HttpClient::HttpClient(QObject * pobj) : QObject(pobj) {
  pNetManager = new QNetworkAccessManager(this);
  // сигнал finished() отправляет по окончании загрузки
  connect(pNetManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(slotFinished(QNetworkReply *)));
}

void HttpClient::download(const QUrl & url) {
  QNetworkRequest request(url);                           // входной объект запроса для pReply
  QNetworkReply * pReply = pNetManager->get(request);     // выполнение запроса
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
HttpClientGui::HttpClientGui(QWidget * pwgt)
  : QWidget(pwgt), idDollarNode("R01235"), idEuroNode("R01239") {
  pClient = new HttpClient(this);
  pProgBar = new QProgressBar;
  pHeaderNameDollar = new QLabel(trUtf8("Доллар США"));
  pHeaderNameEuro = new QLabel(trUtf8("Евро"));
  pHeaderCharCodeDollar = new QLabel("(USD)");
  pHeaderCharCodeEuro = new QLabel("(EUR)");
  pRateDollarLineEdit = new QLineEdit;
  pRateDollarLineEdit->setReadOnly(true);
  pRateEuroLineEdit = new QLineEdit;
  pRateEuroLineEdit->setReadOnly(true);
  pGoButton = new QPushButton("Go");
  strDownloadLink = "http://cbr.ru/scripts/XML_daily.asp";

  connect(pGoButton, SIGNAL(clicked()), SLOT(slotGo()));
  connect(pClient, SIGNAL(downloadProgress(qint64, qint64)),
          this, SLOT(slotDownloadProgress(qint64, qint64)));
  connect(pClient, SIGNAL(done(const QUrl &, const QByteArray &)),
          this, SLOT(slotDone(const QUrl &, const QByteArray &)));

  QGridLayout * pMainLayout = new QGridLayout;
  pMainLayout->addWidget(pHeaderNameDollar, 0, 0);
  pMainLayout->addWidget(pHeaderCharCodeDollar, 0, 1);
  pMainLayout->addWidget(pRateDollarLineEdit, 0, 2);
  pMainLayout->addWidget(pHeaderNameEuro, 1, 0);
  pMainLayout->addWidget(pHeaderCharCodeEuro, 1, 1);
  pMainLayout->addWidget(pRateEuroLineEdit, 1, 2);
  pMainLayout->addWidget(pProgBar, 2, 0);
  pMainLayout->addWidget(pGoButton, 3, 0);

  setLayout(pMainLayout);
}

// запуск процесса загрузки
void HttpClientGui::slotGo() {
  pClient->download(QUrl(strDownloadLink));
}

// отображение процесса загрузки
void HttpClientGui::slotDownloadProgress(qint64 nReceived, qint64 nTotal) {
  // nTotal -- размер загружаемого файла
  if (nTotal <= 0) {
    slotError();
    return;
  }
  pProgBar->setValue(100 * nReceived / nTotal);
}

void HttpClientGui::slotDone(const QUrl & url, const QByteArray & data) {
  // запись в XML файл или отображение данных
  // QDomDocument -- представляет собой XML-документ

  //QDomDocument domDoc;
  // считывание XML-документа в объект QDomDocument
  //domDoc.setContent(data);
  // QDomElement -- содержит корневой элемент XML-документа, который возвращается
  // методом documentElement()
  //QDomElement domElement = domDoc.documentElement();
  // прохождение по всем элементам XML-документа
  //traverseNode(domElement);

  // чтобы поместить XML-документ в SAX-анализатор создаем объект QXmlInputSource
  QXmlInputSource source;
  // и помещаем в него данные XML-документа
  source.setData(data);
  // объект QXmlSimpleReader позволяет анализировать XML-файл
  QXmlSimpleReader reader;
  // установить объект handler который выполняет анализ и отображение XML-документа
  //reader.setContentHandler(handler);
  // запустить анализ XML-файла
  //reader.parse(source);
}

void HttpClientGui::slotError() {
  QMessageBox::critical(0, tr("Error"), tr("An error while is occured"));
}
/*
void HttpClientGui::traverseNode(const QDomNode & node, const QString & flag) {
  QDomNode domNode = node.firstChild();
  while (!domNode.isNull()) {
    if (domNode.isElement()) {
      QDomElement domElement = domNode.toElement();
      if (!domElement.isNull()) {
        if (domElement.tagName() == "Valute") {
          qDebug() << domElement.attribute("ID", "");
          if (domElement.attribute("ID", "") == idDollarNode) {
            traverseNode (domNode);
          }
          else if (domElement.attribute("ID", "") == idEuroNode) {
            traverseNode (domNode, "EUR");
          }
        }
        else {
          qDebug() << "\t" << domElement.tagName()
                   << "\t: " << domElement.text();
          //if ()

        }
      }
    }
    domNode = domNode.nextSibling();
  }
}
*/
void HttpClientGui::setValues(const QDomElement & el) {
  int nominal, value;
  bool ok;
  if (el.tagName() == "Nominal")
    nominal = el.tagName().toInt();
  //if (flag == "USD") {
  //  ;
  //}
}

// AnalizeXML methods
bool AnalizeXML::startElement(const QString &, const QString &,
                              const QString &, const QXmlAttributes & attrs) {

  for (int el = 0; el < attrs.count(); ++el) {
    if (attrs.localName(el) == "id") {
      qDebug() << "Attr: " <<  attrs.value(el);
    }
  }
  return true;
}
