#pragma once

#include <QEventLoop>

#include "src/Network/SSL/sslconnection.h"
#include "src/Network/dtpasenderpool.h"

class SslConnectionPool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit SslConnectionPool(QObject *parent = nullptr, int id = -1);

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

    QString _logPrefix;
    int _id;

    DTPASenderPool _senderPool;
    SslConnection* addConnection(qintptr descriptor);

private:
    QLinkedList<int> _connectionsID;

    int getValidConnectionID();

signals:
    void quit();
};

