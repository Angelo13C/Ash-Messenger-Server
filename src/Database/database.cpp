#include "database.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

void Database::initialize()
{
    //If the database is open succesfully
    if(openDatabase())
        setupDefaultTables();
}

//Get the credentials to access the database from the file
void Database::getCredentials(QString *username, QString *password)
{
    QString credentialsFilePath = Config::readValue("database/credentialsPath");
    //Get the credentials to access the database from the file
    QFile credentialsFile(credentialsFilePath);

    if(!credentialsFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can't open credentials for the database!";
        return;
    }

    //The first line is the username, the second the password
    *username = credentialsFile.readLine();
    *password = credentialsFile.readLine();
    if(username->endsWith('\n'))
        username->chop(1);
    if(password->endsWith('\n'))
        password->chop(1);

    credentialsFile.close();
}

//Connect to the database
bool Database::openDatabase()
{
    //Get the database of type Postgresql
    _db = QSqlDatabase::addDatabase("QPSQL");

    QString dbFilePath = Config::readValue("database/name");
    QString dbHostName = Config::readValue("database/host");
    QString dbPort = Config::readValue("database/port");

    QString dbUserName, dbPassword;
    getCredentials(&dbUserName, &dbPassword);

    //Set the database path
    _db.setDatabaseName(dbFilePath);
    _db.setPort(dbPort.toInt());
    _db.setHostName(dbHostName);
    _db.setUserName(dbUserName);
    _db.setPassword(dbPassword);
    bool ok = _db.open();

    if(!ok)
    {
        qCritical() << "Can't open database!";
    }

    return ok;
}

//Close the database
void Database::closeDatabase()
{
    _db.close();
}

QVector<QStringList> Database::queryResults(QSqlQuery *query)
{
    //If there's been an error while executing the query
    if(_db.lastError().type() != QSqlError::NoError)
    {
        qWarning() << "LOCALDB ERROR: " << _db.lastError();
        qWarning() << query->lastQuery();
    }

    //Get the rows of the result of the query
    QVector<QStringList> results;

    for(int r = 0; query->next(); r++)
    {
        QStringList row;
        for(int i = 0; i < query->record().count(); i++)
        {
            row.append(query->value(i).toString());
        }
        results.append(row);
    }

    return results;
}

//Execute a query
QVector<QStringList> Database::execute(QString queryString)
{
    QSqlQuery query = _db.exec(queryString);

    return queryResults(&query);
}

QVector<QStringList> Database::executeBinded(QString queryString, QStringList values)
{
    QSqlQuery query;
    query.prepare(queryString);
    for(QString value : values)
    {
        query.addBindValue(value);
    }
    query.exec();

    return queryResults(&query);
}

//Create the default tables if they don't exist
void Database::setupDefaultTables()
{
    QFile defaultTables(":/DATABASE/resources/Database/defaultTables.sql");

    if(!defaultTables.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can't open defaultTables.txt for the database!";
        return;
    }

    //Read every line
    QString query;
    QString line;
    while(!defaultTables.atEnd())
    {
        line = defaultTables.readLine();
        //If the readed line has no content, it's the end of the query
        if(line == "\n")
        {
            execute(query);
            query = "";
        }
        else
        {
            if(line.endsWith('\n'))
                line.chop(1);
            query += line;
        }
    }
    query.remove(query.length() - 1, 1);
    execute(query);

    defaultTables.close();
}
