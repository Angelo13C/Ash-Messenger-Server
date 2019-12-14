#pragma once

#include <QObject>
#include <QThread>
#include <QRunnable>
#include <QSslSocket>

#include"DataTransferProtocolAsh/dtpasender.h"

class DTPASenderPool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit DTPASenderPool(QObject *parent = nullptr);

    void run() override;

    void removeConnection(QSslSocket *socket);

    void addConnection(DTPASender *sender);

private:
    QList<DTPASender*> _senders;

    DTPASender *getSender();

signals:

public slots:
};

