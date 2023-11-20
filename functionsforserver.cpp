// functionsforserver.cpp

#include "functionsforserver.h"
#include <QDebug>

int calculateTimeDifference(const QString &startTime,
                            const QString &finishTime) {
  // Convert the start and finish times to QDateTime objects
  QDateTime startDateTime = QDateTime::fromString(startTime, "hh:mm:ss");
  QDateTime finishDateTime = QDateTime::fromString(finishTime, "hh:mm:ss");

  // Calculate the difference in seconds
  qint64 secondsDifference = startDateTime.secsTo(finishDateTime);

  // Convert qint64 to int (if needed)
  int secondsDifferenceInt = static_cast<int>(secondsDifference);

  qDebug() << secondsDifferenceInt;

  return secondsDifferenceInt;
}

int retrieveUserStatistics(const QString &login) {
  int statistics = 0;
  if (DataBase::getStatistics(login, statistics)) {
    qDebug() << "Statistics for user " << login << ": " << statistics;
  } else {
    qDebug() << "Failed to retrieve statistics for user " << login;
  }
  return statistics;
}

QByteArray authHandler(const QStringList &params, QTcpSocket *mTcpSocket) {
  if (params.size() < 2) {
    return "auth_false"; // Invalid number of parameters
  }

  QString login = params.at(0);
  QString password = params.at(1);

  // Check user credentials in the database
  // Assuming your DataBase class has a method like checkCredentials
  if (DataBase::checkCredentials(login, password)) {
    // User found, check if the user is not already in the system
    if (DataBase::isUserOnline(login)) {
      return "auth_err"; // User is already in the system
    } else {
      // Update user status in the database
      DataBase::updateUserStatus(login, "online");
      DataBase::updateUserConnection(mTcpSocket->socketDescriptor(), login);
      return "auth_ok";
    }
  } else {
    return "auth_false"; // User not found or invalid credentials
  }
}

QByteArray startHandler(const QStringList &params, QTcpSocket *mTcpSocket) {
  if (params.size() < 1) {
    return "start_err"; // Invalid number of parameters
  }

  QString startTimeStr = params.at(0);

  // Assuming your DataBase class has a method like startTest
  QString login =
      DataBase::getUserByConnectionDescriptor(mTcpSocket->socketDescriptor());
  qDebug() << login;

  if (!login.isEmpty()) {
    if (DataBase::isTestStarted(login)) {
      return "start_err_restart"; // Test is already started
    } else {
      // Update the test start time in the database
      DataBase::startTest(login, startTimeStr);
      return "start_ok";
    }
  } else {
    return "start_err"; // User not found
  }
}

QByteArray finishHandler(const QStringList &params, QTcpSocket *mTcpSocket) {
  if (params.size() < 1) {
    return "finish_err"; // Invalid number of parameters
  }

  QString finishTimeStr = params.at(0);

  // Assuming your DataBase class has a method like finishTest
  QString login =
      DataBase::getUserByConnectionDescriptor(mTcpSocket->socketDescriptor());

  if (!login.isEmpty()) {
    if (DataBase::isTestStarted(login)) {
      // Calculate the test duration, update statistics, and clear the start
      // time
      QString startTimeStr = DataBase::getTestStartTime(login);
      // Assuming a function to calculate time difference
      int duration = calculateTimeDifference(startTimeStr, finishTimeStr);
      // Assuming a function to update user statistics
      DataBase::updateUserStatistics(login, duration);
      // Clear the test start time
      DataBase::clearTestStartTime(login);

      return "finish_ok";
    } else {
      return "finish_err_refinish"; // Test is not started
    }
  } else {
    return "finish_err"; // User not found
  }
}

QByteArray regHandler(const QStringList &params) {
  if (params.size() < 2) {
    return "reg_err"; // Invalid number of parameters
  }

  QString login = params.at(0);
  QString password = params.at(1);

  if (DataBase::registerUser(login, password)) {
    return "reg_ok"; // Registration successful
  } else {
    return "reg_err"; // Registration failed
  }
}

QByteArray statHandler(const QStringList &params) {
  if (params.size() < 1) {
    return "stat_err"; // Invalid number of parameters
  }

  QString login = params.at(0);

  // Get user statistics from the database
  int statistics = retrieveUserStatistics(login);

  // Return the statistics as a response
  return QByteArray::number(statistics);
}

QByteArray parsing(const QString &data_from_client, QTcpSocket *mTcpSocket) {
  QStringList data_from_client_list =
      data_from_client.trimmed().split(QLatin1Char('\\'));
  QString command = data_from_client_list.front();
  data_from_client_list.pop_front();

  qDebug() << command;
  qDebug() << data_from_client_list;
  if (command == "auth") {
    return authHandler(data_from_client_list, mTcpSocket);
  } else if (command == "start") {
    return startHandler(data_from_client_list, mTcpSocket);
  } else if (command == "finish") {
    return finishHandler(data_from_client_list, mTcpSocket);
  } else if (command == "reg") {
    return regHandler(data_from_client_list);
  } else if (command == "stat") {
    return statHandler(data_from_client_list);
  } else {
    return "unknown_command";
  }
}
