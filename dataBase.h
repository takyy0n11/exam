#pragma once

#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QMutex>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>

class DataBase;

class DataBaseDestroyer {
private:
  DataBase *dataBasePointer;

public:
  ~DataBaseDestroyer() { delete dataBasePointer; }
  void initialize(DataBase *p) { dataBasePointer = p; }
};

class DataBase {
private:
  static DataBase *p_instance;
  static DataBaseDestroyer destroyer;

  static QMap<int, QString> connectionToUserMap;
  static QMutex connectionMapMutex;

  static QSqlDatabase db;

protected:
  DataBase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Test.db");

    if (!db.open()) {
      qDebug() << "Error opening the database: " << db.lastError().text();
    } else {
      // Check if the 'Users' table exists; if not, create it
      QSqlQuery query;
      if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("
                      "  Login TEXT PRIMARY KEY,"
                      "  Password TEXT,"
                      "  Status TEXT,"
                      "  Started TEXT,"
                      "  Statistics INTEGER"
                      ");")) {
        qDebug() << "Error creating 'Users' table: "
                 << query.lastError().text();
      }
    }
  }

  friend class DataBaseDestroyer;

public:
  static DataBase *getInstance() {
    if (!p_instance) {
      p_instance = new DataBase();

      destroyer.initialize(p_instance);
    }
    return p_instance;
  }

  static bool checkCredentials(const QString &login, const QString &password) {
    QSqlQuery query;
    query.prepare(
        "SELECT * FROM Users WHERE Login = :login AND Password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
      return true; // Credentials are valid
    }
    return false; // Credentials are invalid
  }

  static bool isUserOnline(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT Status FROM Users WHERE Login = :login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
      QString status = query.value(0).toString();
      return !status.isEmpty(); // User is online if the status is not empty
    }
    return false; // User not found or an error occurred
  }

  static void updateUserStatus(const QString &login, const QString &status) {
    QSqlQuery query;
    query.prepare("UPDATE Users SET Status = :status WHERE Login = :login");
    query.bindValue(":status", status);
    query.bindValue(":login", login);
    query.exec();
  }

  static void startTest(const QString &login, const QString &startTime) {
    QSqlQuery query;
    query.prepare("UPDATE Users SET Started = :startTime WHERE Login = :login");
    query.bindValue(":startTime", startTime);
    query.bindValue(":login", login);
    query.exec();
  }

  static bool isTestStarted(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT Started FROM Users WHERE Login = :login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
      QString startTime = query.value(0).toString();
      return !startTime
                  .isEmpty(); // Test is started if the start time is not empty
    }
    return false; // User not found or an error occurred
  }

  static QString getTestStartTime(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT Started FROM Users WHERE Login = :login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
      return query.value(0).toString();
    }
    return ""; // User not found or an error occurred
  }

  static void updateUserStatistics(const QString &login, int duration) {
    QSqlQuery query;
    query.prepare("UPDATE Users SET Statistics = Statistics + :duration WHERE "
                  "Login = :login");
    query.bindValue(":duration", duration);
    query.bindValue(":login", login);
    query.exec();
  }

  static void clearTestStartTime(const QString &login) {
    QSqlQuery query;
    query.prepare("UPDATE Users SET Started = '' WHERE Login = :login");
    query.bindValue(":login", login);
    query.exec();
  }

  static QString getUserByConnectionDescriptor(int descriptor) {
    QMutexLocker locker(&connectionMapMutex);
    return connectionToUserMap.value(descriptor, "");
  }

  // Add a method to update the map when a user logs in
  static void updateUserConnection(int descriptor, const QString &login) {
    QMutexLocker locker(&connectionMapMutex);
    connectionToUserMap[descriptor] = login;
  }

  static bool registerUser(const QString &login, const QString &password) {
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Users (Login, Password, Statistics) VALUES (:login, :password, 0)");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (query.exec()) {
      qDebug() << "User registered successfully";
      return true;
    } else {
      qDebug() << "Error registering user: " << query.lastError().text();
      return false;
    }
  }

  static bool getStatistics(const QString &login, int &statistics) {
    QSqlQuery query;
    query.prepare("SELECT Statistics FROM Users WHERE Login = :login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
      statistics = query.value(0).toInt();
      return true;
    } else {
      qDebug() << "Error retrieving statistics: " << query.lastError().text();
      return false;
    }
  }
};
