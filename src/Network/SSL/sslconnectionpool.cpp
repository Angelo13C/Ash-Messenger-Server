#include "sslconnectionpool.h"

#include <QTcpSocket>

SslConnectionPool::SslConnectionPool(QObject *parent, int id)
    : QObject(parent), _logPrefix("Pool " + QString::number(id)), _id(id)
{

}

//Initialize the object on the thread
void SslConnectionPool::initialize(QThread *thread)
{
    connect(thread, &QThread::started, this, &SslConnectionPool::run);
}

//Run the pool
void SslConnectionPool::run()
{
    qInfo().noquote() << _logPrefix << "running";
    _senderPool.run();

    loop = new QEventLoop();
    connect(this, &SslConnectionPool::quit, loop, &QEventLoop::quit);
    loop->exec();

    qInfo().noquote() << _logPrefix << "quitting";
}

//When a new connection is received
void SslConnectionPool::connectionPending(qintptr descriptor, SslConnectionPool *pool)
{
    if(pool != this)
        return;

    qInfo().noquote() << _logPrefix << "accepting descriptor:" << descriptor;

    SslConnection *connection = addConnection(descriptor);

    if(!connection)
    {
        qInfo().noquote() << _logPrefix << "can't add connection";
        return;
    }
}

//Add a new connection to the pool and setup that
SslConnection* SslConnectionPool::addConnection(qintptr descriptor)
{
    SslConnection *connection = new SslConnection(nullptr, _id, getValidConnectionID());
    connection->setObjectName("Connection " + QString::number(_connections.size()));

    connection->setSocket(descriptor);

    connect(connection, &SslConnection::connected, this, &SslConnectionPool::connectionStarted);
    connect(connection, &SslConnection::disconnected, this, &SslConnectionPool::connectionFinished);

    _connections.append(connection);

    _senderPool.addConnection(connection->getSender());

    return connection;
}

//Get a valid connection ID for the pool
int SslConnectionPool::getValidConnectionID()
{
    int id = 0;

    auto i = _connectionsID.begin();
    //Cycle through all the connections ID already in use
    for (; i!= _connectionsID.end(); i++)
    {
        if(*i == id)
            id++;
        else
            break;
    }

    _connectionsID.insert(i, id);

    return id;
}

//Get how many connection is the pool handling
int SslConnectionPool::connectionCount()
{
    int count = _connections.count();
    return count;
}

//When the connection is started
void SslConnectionPool::connectionStarted()
{
    SslConnection *connection = static_cast<SslConnection*>(sender());

    if (!connection)
        return;

    qInfo() << _logPrefix << "connection started" << connection->getID();
}

//When the connection is ended
void SslConnectionPool::connectionFinished()
{
    SslConnection *connection = static_cast<SslConnection*>(sender());

    //Remove the connection from the pool
    _senderPool.removeConnection(connection->getSocket());
    //Mark the ID of the disconnected connection as usable
    _connectionsID.removeAll(connection->getID());

    //Remove connection from the list and delete the connection
    _connections.removeAll(connection);
    connection->deleteLater();

    qInfo().noquote() << _logPrefix << "has removed connection" << connection->getID();
}

