#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>

#include <iostream>

#include <QSslSocket>
#include <QFile>
#include <QSslKey>

#include "src/Network/DataTransferProtocolAsh/dtparequest.h"
#include "src/Network/DataTransferProtocolAsh/dtpareceiver.h"
#include "src/Network/DataTransferProtocolAsh/dtpasender.h"

#include "src/Network/connectionsmanager.h"

class SslConnection : public QObject
{
    Q_OBJECT
public:
    explicit SslConnection(QObject *parent = nullptr);
    ~SslConnection();

    void setSocket(qintptr descriptor);
    QSslSocket *getSocket();

private:
    QSslSocket *_socket;

    QString _cachedPacket = "";
    QByteArray _cachedBytePacket = "";

    void setupSsl(QSslSocket *socket);

    DTPASender *_sender;
    DTPAReceiver *_receiver;

    QString readSocket();

public:
    DTPASender *getSender();

signals:
    void connected();
    void disconnected();
    void addToCachedRequests(DTPARequest *request);

public slots:
    void onConnect();
    void onDisconnect();
    void onReadyRead();
    void onBytesWritten(qint64 bytes);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void onError(QAbstractSocket::SocketError socketError);
    void onSslError(QList<QSslError> socketErrors);
};

