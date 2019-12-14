#pragma once

#include <QObject>

#include <QThread>
#include <QMutex>

#include "src/Network/SSL/sslconnection.h"
#include "src/Network/dtpasenderpool.h"

class SslConnectionPool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit SslConnectionPool(QObject *parent = nullptr);

    int connectionCount();

    void run() override;

public slots:
    void connectionPending(qintptr descriptor, SslConnectionPool *pool);
    void connectionStarted();
    void connectionFinished();


private:
    QList<SslConnection*> _connections;
    QMutex mutex;

    QEventLoop *loop;

    SslConnection* addConnection(qintptr descriptor);

    DTPASenderPool _senderPool;

signals:
    void quit();
};

