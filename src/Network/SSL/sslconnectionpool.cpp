#include "sslconnectionpool.h"

#include <QTcpSocket>

#include <QEventLoop>
#include <QRunnable>
#include <QMutexLocker>
#include <QReadWriteLock>

#include <QList>

SslConnectionPool::SslConnectionPool(QObject *parent) : QObject(parent)
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
    qDebug() << this->objectName() << " running ";
    _senderPool.run();

    loop = new QEventLoop();
    connect(this, &SslConnectionPool::quit, loop, &QEventLoop::quit);
    loop->exec();

    qDebug() << "Connection Pool quitting";
}
#include "src/performancebenchmark.h"   //DELETE
//When a new connection is received
void SslConnectionPool::connectionPending(qintptr descriptor, SslConnectionPool *pool)
{
    if(pool != this)
        return;

    qDebug() << "Accepting descriptor: " << descriptor;

    SslConnection *connection = addConnection(descriptor);

    if(!connection)
    {
        qDebug() << "Can't add connection";
        return;
    }
}

//Add a new connection to the pool and setup that
SslConnection* SslConnectionPool::addConnection(qintptr descriptor)
{
    SslConnection *connection = new SslConnection(nullptr, objectName());
    connection->setObjectName("Connection " + QString::number(_connections.size()));

    connection->setSocket(descriptor);

    connect(connection, &SslConnection::connected, this, &SslConnectionPool::connectionStarted);
    connect(connection, &SslConnection::disconnected, this, &SslConnectionPool::connectionFinished);

    _connections.append(connection);

    _senderPool.addConnection(connection->getSender());

    return connection;
}

//Get how many connection is the pool handling
int SslConnectionPool::connectionCount()
{
    QMutexLocker locker(&mutex);

    int count = _connections.count();
    return count;
}

//When the connection is started
void SslConnectionPool::connectionStarted()
{
    SslConnection *connection = static_cast<SslConnection*>(sender());

    if (!connection)
        return;

    qDebug() << this->objectName() << " connection started " << connection;
}

//When the connection is ended
void SslConnectionPool::connectionFinished()
{
    SslConnection *connection = static_cast<SslConnection*>(sender());

    qDebug() << this->objectName() << connection->objectName() << " finished ";

    //Remove the connection from the sender pool
    _senderPool.removeConnection(connection->getSocket());

    //Remove connection from the list and delete the connection
    _connections.removeAll(connection);
    connection->deleteLater();

    qDebug() << this->objectName() << connection->objectName() << " removed ";
}

