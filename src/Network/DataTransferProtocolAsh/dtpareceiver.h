#pragma once

#include <QVector>
#include <QList>

#include "dtpa.h"
#include "dtparequest.h"
#include "dtpaform.h"
#include "dtpareceivermanager.h"
#include "dtpasender.h"

class DTPAReceiver
{
public:
    DTPAReceiver(DTPASender *connection);

    static DTPARequest fragmentToRequest(QString fragment);

    static QList<DTPARequest> packetToRequests(QString packet);

    void addToCachedRequests(DTPARequest request);
    void removeFromCachedRequests(DTPARequest *request);

private:
    QVector<DTPARequest> _cachedRequests;
    DTPASender *_connection;

};

