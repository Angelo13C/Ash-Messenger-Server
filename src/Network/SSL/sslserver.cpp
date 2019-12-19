#include "sslserver.h"

#define RED "\033[1;31m"

SslServer::SslServer()
{
    qRegisterMetaType<qintptr>("qintptr");
}

SslServer::~SslServer()
{
    stop();
}

//Start the server
void SslServer::start(const QHostAddress &address, quint16 port)
{
    if(port == 0)   //If no port is given, get the config one
        port = Config::readValue("network/port").toUShort();

    qDebug() << "Starting server!";

    if(!listen(address, port))
    {
        qDebug() << RED << "Can't listen into the specified port!";
        return;
    }

    int threadCount = -1;

    if(threadCount == -1)
        threadCount = QThread::idealThreadCount();

    else if(threadCount < 2)
        qDebug() << RED << "Can't run a pool on only" << threadCount << "threads";

    for(int8_t i = 0; i < (threadCount - _otherThreadCount) / 2; i++)
    {
        SslServer::createPool("Pool " + QString::number(i));
    }

}

//Stop the server
void SslServer::stop()
{
    qWarning() << RED << "Stopping server!";
    for(SslConnectionPool *pool : _pools)
    {
        deletePool(pool);
    }
    close();
}

//Create a new connection pool
void SslServer::createPool(QString id)
{
    SslConnectionPool* pool = new SslConnectionPool();
    QThread *thread = new QThread;

    pool->setObjectName(id);
    thread->setObjectName(id);

    pool->initialize(thread);
    pool->moveToThread(thread);

    thread->start();

    //Set the priority of the networking thread
    int priority = Config::readValue("network/priority").toInt();
    thread->setPriority(static_cast<QThread::Priority>(priority));

    SslServer::addPool(pool);
}

//Add a connection pool to the mechanism
void SslServer::addPool(SslConnectionPool *pool)
{
    _pools.append(pool);

    connect(this, &SslServer::connectionPending, pool, &SslConnectionPool::connectionPending);
}

//Delete a pool
void SslServer::deletePool(SslConnectionPool *pool)
{
    QThread *thread = pool->thread();
    if(thread != nullptr)
    {
        thread->quit();
        thread->wait();
        thread->deleteLater();
        pool->deleteLater();
    }
}

//When a new connection is received
void SslServer::incomingConnection(qintptr descriptor)
{
    SslConnectionPool* selectedPool = nullptr;

    //Select the pool with less connections
    foreach(SslConnectionPool* pool, _pools)
    {
        if(!selectedPool)
            selectedPool = pool;

        else if(pool->connectionCount() < selectedPool->connectionCount())
            selectedPool = pool;
    }

    emit connectionPending(descriptor, selectedPool);
}
