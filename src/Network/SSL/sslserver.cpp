#include "sslserver.h"

#define RED "\033[1;31m"

SslServer::SslServer()
{

}

//Start the server
void SslServer::start(const QHostAddress &address, quint16 port)
{
    if(port == 0)   //If no port is given, get the config one
        port = Config::readValue("networking/port").toUShort();

    QThreadPool::globalInstance()->setMaxThreadCount(4);

    std::cout << "Starting server!" << std::endl;

    if(!listen(address, port))
    {
        std::cout << RED << "Can't listen into the specified port!" << std::endl;
        return;
    }

    for(int8_t i = 0; i < QThreadPool::globalInstance()->maxThreadCount() / 2; i++)
    {
        SslServer::createPool();
    }

}

//Stop the server
void SslServer::stop()
{
    std::cout << RED << "Closing server!" << std::endl;
    close();
}

//Create a new connection pool
void SslServer::createPool()
{
    SslConnectionPool* pool = new SslConnectionPool();
    SslServer::addPool(pool);
}

//Add a connection pool to the mechanism
void SslServer::addPool(SslConnectionPool *pool)
{
    _pools.append(pool);

    connect(this, &SslServer::connectionPending, pool, &SslConnectionPool::connectionPending);

    QThreadPool::globalInstance()->start(pool);
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
