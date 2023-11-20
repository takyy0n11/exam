#include "mytcpserver.h"
#include <QCoreApplication>

DataBase *DataBase::p_instance = 0;
DataBaseDestroyer DataBase::destroyer;
QSqlDatabase DataBase::db;
QMap<int, QString> DataBase::connectionToUserMap;
QMutex DataBase::connectionMapMutex;

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  DataBase::getInstance();
  MyTcpServer myserv;
  return a.exec();
}
