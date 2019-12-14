#pragma once

#include <QCoreApplication>
#include <QObject>

#include <QSslSocket>

#include "dtpa.h"
#include "dtparequest.h"

class DTPASender : public QObject
{
Q_OBJECT

public:
    explicit DTPASender(QObject *parent = nullptr, QSslSocket *socket = nullptr);

    QString getPacketFromQueue();

    int getQueueLength();
    QSslSocket *getSocket();

    void sendPacket();
    DTPARequest *getHighestPriorityRequest(int *outRequestIndex);
    int16_t getValidRequestID();

    void freeAnswerID(DTPARequest *request, bool *deleteRequest);

    typedef std::function<void(DTPARequest *, DTPASender *, int)> funcRequestAnswer;
    void callRequestAnswer(DTPARequest *request);
    typedef std::function<void(DTPARequest *, DTPASender *)> funcRequestEnd;
    void callRequestEnd(DTPARequest *request);
    void sendRequest(DTPARequest *request, funcRequestAnswer funcAnswer = nullptr, funcRequestEnd funcEnd = nullptr);
private:
    QMap<int, funcRequestAnswer> _funcAnswerRequest;
    QMap<int, funcRequestEnd> _funcEndRequest;

public slots:
    void sendRequestSignal(DTPARequest *request);

private:
    QList<DTPARequest*> _requestQueue;
    bool _requestIDs[10000] = {false};

    QSslSocket *_socket = nullptr;
};
