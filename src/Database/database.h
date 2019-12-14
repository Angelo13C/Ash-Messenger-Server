#pragma once
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QFile>

#include "src/Other/config.h"

namespace Database
{
static QSqlDatabase _db;

void initialize();
void getCredentials(QString *username, QString *password);
bool openDatabase();
void closeDatabase();

void setupDefaultTables();

QVector<QStringList> queryResults(QSqlQuery *query);
QVector<QStringList> execute(QString queryString);
QVector<QStringList> executeBinded(QString queryString, QStringList values);
};



/*#pragma once
#include <iostream>
#include <string>
#include <QFile>
#include <pqxx/pqxx>

#include "config.h"

namespace Database
{
void getCredentials(QString *username, QString *password);
void initialize();

pqxx::result execute(QString workString);
pqxx::result executeParameterized(QString id, QString query, QStringList parameters);
void executeTransaction(QStringList workStringList);
QString resToQString(pqxx::result, uint x, uint y);
void setupDefaultTables();

static pqxx::connection *_connection;
}*/
