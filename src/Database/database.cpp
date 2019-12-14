
/*#include "database.h"

//Get the credentials to access the database from the file
void Database::getCredentials(QString *username, QString *password)
{
    //Get the credentials to access the database from the file
    QFile credentialsFile(":Database/Database/credentials.txt");

    if(!credentialsFile.open(QIODevice::ReadOnly))
    {
        std::cout << "Can't open credentials.txt for the database!";
        return;
    }

    //The first line is the username, the second the password
    *username = credentialsFile.readLine();
    *password = credentialsFile.readLine();

    credentialsFile.close();
}


//Initialize the database
void Database::initialize()
{
    //Get the credentials to access the database from the file
    QString username, password;
    getCredentials(&username, &password);

    const std::string host = Config::readValue("database/host").toStdString();
    const std::string port = Config::readValue("database/port").toStdString();
    const std::string dbName = Config::readValue("database/name").toStdString();

    //String to connect to the database
    std::string con_string("host=" + host + " port=" + port + " dbname=" + dbName +
                           " user=" + username.toStdString() + " password=" + password.toStdString());

    //Create the connection to the database
    _connection = new pqxx::connection(con_string.c_str());

    //Create the default tables if they don't exist
    setupDefaultTables();
}

//Get the qstring from a result and the indexes
QString Database::resToQString(pqxx::result result, uint x, uint y)
{
    std::string resultStd = "";
    //If it's null, it crashes
    if(result.size() > 0 && result.at(x).size() > 0)
    {
        if(!result.at(x).at(y).is_null())
            resultStd = result.at(x).at(y).as<std::string>();
    }

    return QString::fromStdString(resultStd);
}

//Execute a query
pqxx::result Database::execute(QString workString)
{
    //If there isn't ; as last character warns the developer
    if(workString.at(workString.length() - 1) != ';')
    {
        std::cout << "No ; at the end of the query";
    }

    pqxx::work work(*_connection);

    //Execute the work
    pqxx::result result = work.exec(workString.toStdString());

    //Commit the execution
    work.commit();

    return result;
}

//Execute a query removing the possibility of SQL INJECTION
pqxx::result Database::executeParameterized(QString id, QString query, QStringList parameters)
{
    _connection->prepare(id.toStdString(), query.toStdString());

    pqxx::work work(*_connection);

    //Dynamic paramatization of the prepared query
    pqxx::prepare::invocation invocation = work.prepared(id.toStdString());
    for(QString par : parameters)
        invocation(par.toStdString());

    //Execute the work
    pqxx::result result = invocation.exec();
    //Commit the execution
    work.commit();

    return result;
}
*/
/*
//Execute a query
void Database::executeTransaction(QStringList workStringList)
{
    //If there isn't ; as last character, it won't execute the query
    //if(workString.at(workString.length() - 1) != ';')
    {
        //std::cout << "No ; at the end of the query";
        //return;
    }

    pqxx::work work(*_connection);

    work.exec("BEGIN;");

    foreach (QString workString, workStringList) {
        //try {
            work.exec(workString.toStdString());
        //} catch (const pqxx::sql_error& e) {
            //work.exec("ROLLBACK;");
        //}
    }
    work.exec("COMMIT;");

    //Execute the work
    //pqxx::result result = work.exec(workString.toStdString());

    //return result;
}*/
/*
//Create the default tables if they don't exist
void Database::setupDefaultTables()
{
    QFile defaultTables(":Database/Database/defaultTables.sql");

    if(!defaultTables.open(QIODevice::ReadOnly))
    {
        std::cout << "Can't open defaultTables.sql for the database!";
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
            query.remove(query.length() - 1, 1);
            execute(query);
            query = "";
        }
        else
            query += line;
    }
    query.remove(query.length() - 1, 1);
    execute(query);

    defaultTables.close();
}
*/

#include "database.h"

void Database::initialize()
{
    //If the database is open succesfully
    if(openDatabase())
        setupDefaultTables();
}

//Get the credentials to access the database from the file
void Database::getCredentials(QString *username, QString *password)
{
    //Get the credentials to access the database from the file
    QFile credentialsFile(":/DATABASE/resources/Database/credentials.txt");

    if(!credentialsFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't open credentials.txt for the database!";
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
    //Get the database fo type Postgresql
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
        qDebug() << "Can't open database!";
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
        qDebug() << "LOCALDB ERROR: " << _db.lastError();
        qDebug() << query->lastQuery();
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
        qDebug() << "Can't open defaultTables.txt for the database!";
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
            if(line.at(line.length() - 1) == '\n')
                line.chop(1);
            query += line;
        }
    }
    query.remove(query.length() - 1, 1);
    execute(query);

    defaultTables.close();
}
