#pragma once

#include <iostream>
#include <QTcpServer>
#include <QThreadPool>

#include "src/Network/SSL/sslconnectionpool.h"

#include "src/Other/config.h"

#define SERVER_ADDRESS_TOLISTEN QHostAddress::Any

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    SslServer();

    void start(const QHostAddress &address = SERVER_ADDRESS_TOLISTEN, quint16 port = 0);
    void stop();


public slots:
    void incomingConnection(qintptr descriptor);

private:
    QList<SslConnectionPool*> _pools;

    void createPool();
    void addPool(SslConnectionPool *pool);

signals:
    void connectionPending(qintptr descriptor, SslConnectionPool *pool);
};

