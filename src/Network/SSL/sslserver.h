#pragma once

#include <QTcpServer>

#include "src/Network/SSL/sslconnectionpool.h"

#define SERVER_ADDRESS_TOLISTEN QHostAddress::Any

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    SslServer();
    ~SslServer();

    void start(const QHostAddress &address = SERVER_ADDRESS_TOLISTEN, quint16 port = 0);
    void stop();

public slots:
    void incomingConnection(qintptr descriptor);

private:
    QList<SslConnectionPool *> _pools;

    static constexpr int _otherThreadCount = 1;

    void createPool(int id);
    void addPool(SslConnectionPool *pool);
    void deletePool(SslConnectionPool *pool);

signals:
    void connectionPending(qintptr descriptor, SslConnectionPool *pool);
};

