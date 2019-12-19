#pragma once

#include <QEventLoop>

#include "src/Network/SSL/sslconnection.h"
#include "src/Network/dtpasenderpool.h"

class SslConnectionPool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit SslConnectionPool(QObject *parent = nullptr);

    void initialize(QThread *thread);
    void run() override;

    int connectionCount();

public slots:
    void connectionPending(qintptr descriptor, SslConnectionPool *pool);
    void connectionStarted();
    void connectionFinished();


private:
    QList<SslConnection*> _connections;

    QEventLoop *loop;

    SslConnection* addConnection(qintptr descriptor);

    DTPASenderPool _senderPool;

signals:
    void quit();
};

