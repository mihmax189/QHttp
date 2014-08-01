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
HttpClientGui::HttpClientGui(QWidget * pwgt) : QWidget(pwgt) {
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
  // объект AnalizeXML осуществляет прохождение по XML-документу
  AnalizeXML handler;
  handler.getCursDollarLine(pRateDollarLineEdit);
  handler.getCursEuroLine(pRateEuroLineEdit);
  // чтобы поместить XML-документ в SAX-анализатор создаем объект QXmlInputSource
  QXmlInputSource source;
  // и помещаем в него данные XML-документа
  source.setData(data);
  // объект QXmlSimpleReader позволяет анализировать XML-файл
  QXmlSimpleReader reader;
  // установить объект handler который выполняет анализ и отображение XML-документа
  reader.setContentHandler(&handler);
  // запустить анализ XML-файла
  reader.parse(source);
}

void HttpClientGui::slotError() {
  QMessageBox::critical(0, tr("Error"), tr("An error while is occured"));
}

// AnalizeXML methods -- методы, которые вызываются при
// прохождении XML-документу

AnalizeXML::AnalizeXML() : QXmlDefaultHandler(),
                           idDollarNode("R01235"), idEuroNode("R01239") {
  nominal = 0;
  value = 0.0;
}
// вызывается, когда при считывании XML-документа встречается открытие тега
bool AnalizeXML::startElement(const QString &, const QString &,
                              const QString &, const QXmlAttributes & attrs) {
  // attrs -- список атрибутов тега
  for (int el = 0; el < attrs.count(); ++el) {
    if (attrs.localName(el) == "ID") {
      if (attrs.value(el) == idDollarNode) {
        currentCharCode = attrs.value(el);
      }
      else if (attrs.value(el) == idEuroNode) {
        currentCharCode = attrs.value(el);
      }
    }
  }
  return true;
}

// записывает содержимое текущего тега в m_strText
bool AnalizeXML::characters(const QString & strText) {
  m_strText = strText;
  return true;
}

// вызывается всегда, когда при чтении встречается закрытие тега
bool AnalizeXML::endElement(const QString &, const QString &,
                            const QString & str) {
  // str -- имя тега
  if (currentCharCode == idDollarNode) {
    if (str == "Nominal") {
      nominal = m_strText.toInt();
    }
    if (str == "Value") {
      bool ok;
      value = m_strText.toDouble(&ok);
    }
    if (nominal != 0 && value != 0.0) {
      cursDollarLine->setText(QString::number(value / nominal));
      nominal = 0;
      value = 0.0;
      currentCharCode = "";
    }
  }

  if (currentCharCode == idEuroNode) {
    if (str == "Nominal") {
      nominal = m_strText.toInt();
    }
    if (str == "Value") {
      bool ok;
      value = m_strText.toDouble(&ok);
    }
    if (nominal != 0 && value != 0.0) {
      cursEuroLine->setText(QString::number(value / nominal));
      nominal = 0;
      value = 0.0;
      currentCharCode = "";
    }
  }
  return true;
}

// вызывается, если не удается проанализировать XML-документ
bool AnalizeXML::fatalError(const QXmlParseException & exception) {
  qDebug() << "Line: " << exception.lineNumber();
  qDebug() << "Column: " << exception.columnNumber();
  qDebug() << "Message: " << exception.message();
  return false;
}

void AnalizeXML::getCursDollarLine(QLineEdit * ptr) {
  cursDollarLine = ptr;
}
void AnalizeXML::getCursEuroLine(QLineEdit * ptr) {
  cursEuroLine = ptr;
}
