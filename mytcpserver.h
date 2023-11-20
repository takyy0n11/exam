#pragma once

// #include "dataBase.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <QByteArray>
#include <QDebug>
#include <QtNetwork>
#include <list>
// #include "functions.h"
#include "functionsforserver.h"

/**
 * \authors авторы
 * \date дата написания
 * \bug описание багов
 * \todo что доделать
 * \version указание версии
 * \example example_file.txt
 * \warning ограничения
 * \brief The MyTcpServer class - класс для описания работы сервера
 *
 * Класс на основе QTcpServer, который ....
 */

class MyTcpServer : public QObject {
public:
  explicit MyTcpServer(QObject *parent = nullptr);
  ~MyTcpServer();
public slots:
  /**
   * \brief slotNewConnection - сло для ...
   *
   * подробное описание слота...
   */
  void slotNewConnection();
  void slotClientDisconnected();
  void slotServerRead();

private:
  QTcpServer *mTcpServer;
  QTcpSocket *mTcpSocket;
  // int server_status;
};
