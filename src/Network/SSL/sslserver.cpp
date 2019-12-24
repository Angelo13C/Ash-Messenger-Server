#include "sslserver.h"

#include "src/Other/config.h"

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

    qInfo() << "Starting server!";

    if(!listen(address, port))
    {
        qFatal("Can't listen into the specified port!");
    }

    int threadCount = -1;

    if(threadCount == -1)
        threadCount = QThread::idealThreadCount();

    else if(threadCount < 2)
        qWarning() << "Can't run a pool on only" << threadCount << "threads";

    for(int8_t i = 0; i < (threadCount - _otherThreadCount) / 2; i++)
    {
        SslServer::createPool(i);
    }

}

//Stop the server
void SslServer::stop()
{
    qCritical() << "Stopping server!";
    for(SslConnectionPool *pool : _pools)
    {
        deletePool(pool);
    }
    close();
}

//Create a new connection pool
void SslServer::createPool(int id)
{
    SslConnectionPool* pool = new SslConnectionPool(nullptr, id);
    QThread *thread = new QThread;

    pool->setObjectName("Pool " + QString::number(id));
    thread->setObjectName("Pool " + QString::number(id));

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
    for(SslConnectionPool* pool : _pools)
    {
        if(!selectedPool)
            selectedPool = pool;

        else if(pool->connectionCount() < selectedPool->connectionCount())
            selectedPool = pool;
    }

    emit connectionPending(descriptor, selectedPool);
}
