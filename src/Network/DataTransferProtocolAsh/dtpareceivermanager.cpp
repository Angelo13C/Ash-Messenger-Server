#include "dtpareceivermanager.h"

#include "dtpa.h"

DTPAReceiverManager::DTPAReceiverManager()
{

}

/* SINGLETON */
DTPAReceiverManager* DTPAReceiverManager::getInstance()
{
    static DTPAReceiverManager *instance = new DTPAReceiverManager();
    return instance;
}
/* END SINGLETON */

//Register a function to a command of a request
void DTPAReceiverManager::signCommandFunction(DTPARequest::Command command, funcCommand function)
{
    _commandFunctions.insertMulti(command, function);
}

//Call the function based on the command of the received request
void DTPAReceiverManager::callRequestCommand(DTPASender *connection, DTPARequest *request, bool *deleteRequest)
{
    int foundFunctionsCount = 0;

    QMap<DTPARequest::Command, funcCommand>::iterator i;
    /* Iterate through every signed command function and find the one with the
       corrispondent command of the received request */
    for(i = _commandFunctions.begin(); i != _commandFunctions.end(); i++)
    {
        if(i.key() == request->getCommand())
        {
            foundFunctionsCount++;

            funcCommand f = i.value();
            f(connection, request, deleteRequest);
        }
    }
    //If no function with that command is been found, show error
    if(foundFunctionsCount == 0)
    {
        //Delete the request if there's no function listening to it
        *deleteRequest = true;
    }
}
