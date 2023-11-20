#pragma once

#include "dataBase.h"
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QTcpSocket>

/**
 * \brief parsing - функция, которая по введенному названию
 * и параметрам функции возращает результат.
 * \param [in] data_from_client - строка, которая приходит от клиента
 * в формате....
 * \return Возвращает результат работы функции, которая была подана на вход.
 *
 * \example указать_файл.txt
 */
QByteArray parsing(const QString &data_from_client, QTcpSocket *mTcpSocket);

QByteArray auth(QString log, QString pass);

QByteArray reg(QString log, QString pass, QString mail);
