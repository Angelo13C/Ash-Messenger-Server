#pragma once

#include <QDebug>
#include <QMap>

#include <functional>

#include "dtpa.h"
#include "dtparequest.h"
#include "dtpasender.h"

class DTPAReceiverManager
{
public:
    DTPAReceiverManager();

    void callRequestCommand(DTPASender *connection, DTPARequest *request, bool *deleteRequest);

    /* SINGLETON */
    static DTPAReceiverManager* getInstance();

    typedef std::function<void(DTPASender *, DTPARequest *, bool *)> funcCommand;
    void signCommandFunction(DTPARequest::Command command, funcCommand function);
private:
    QMap<DTPARequest::Command, funcCommand> _commandFunctions;
};

