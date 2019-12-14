#include "dtpasenderpool.h"

DTPASenderPool::DTPASenderPool(QObject *parent) : QObject(parent)
{
}

void DTPASenderPool::addConnection(DTPASender *sender)
{
    _senders.append(sender);
}

void DTPASenderPool::removeConnection(QSslSocket *socket)
{
    for(int i = 0; i < _senders.length(); i++)
    {
        if(_senders.at(i)->getSocket() == socket)
        {
            _senders.removeAt(i);
            break;
        }
    }
}

DTPASender *DTPASenderPool::getSender()
{
    if(_senders.isEmpty())
        return nullptr;

    DTPASender *sender = nullptr;

    bool firstLoop = true;
    static int startIndex = 0;

    int i = startIndex;

    for(; i < _senders.length() || firstLoop; i++)
    {
        if(i > startIndex && !firstLoop)
            break;

        if(firstLoop && i >= _senders.length())
        {
            firstLoop = false;
            i = 0;
        }
        //If there's at least 1 request in the queue
        if(_senders.at(i)->getQueueLength() != 0)
        {
            sender = _senders.at(i);
            break;
        }
    }

    return sender;
}

void DTPASenderPool::run()
{
    //Sleep 1 second before starting to send data;
    QThread::sleep(1);

    while(true)
    {
        DTPASender *sender = getSender();

        if(sender == nullptr)
            QThread::msleep(1);
        else
            sender->sendPacket();

        QCoreApplication::processEvents();
    }
}
