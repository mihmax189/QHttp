#include <QtGui>
#include "HttpClientGui.hpp"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  HttpClientGui client;
  client.show();
  return app.exec();
}
