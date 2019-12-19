#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

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
