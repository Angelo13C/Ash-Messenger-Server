#include "dtpasender.h"

//Initialize the sender
DTPASender::DTPASender(QObject *parent, QSslSocket *socket)
    : _socket(socket)
{
    Q_UNUSED(parent)
}

//Add a request to the queue of request to send
void DTPASender::sendRequest(DTPARequest *request, funcRequestAnswer funcAnswer, funcRequestEnd funcEnd)
{
    emit sendRequestSignal(request);

    //Add to the map of the functions to call
    if(funcAnswer != nullptr)
        _funcAnswerRequest.insert(request->getID(), funcAnswer);
    //Add to the map of the functions to call
    if(funcEnd != nullptr)
        _funcEndRequest.insert(request->getID(), funcEnd);
}

//Add a request to the queue of request to send
void DTPASender::sendRequestSignal(DTPARequest *request)
{
    request->setID(getValidRequestID());
    _requestQueue.append(new DTPARequest(*request));
}

//Get a packet based on the requests that there are in the queue
QString DTPASender::getPacketFromQueue()
{
    QString packet = "";

    //Get the fragment
    if(!_requestQueue.isEmpty())
    {
        DTPARequest *request;
        do
        {
            //Get the Request
            int requestIndex;
            request = getHighestPriorityRequest(&requestIndex);

            //If the request is ended, remove from the list and pass to the other request
            bool requestEnd;
            do
            {
                //Get the fragment of a max size of 4096 - the current packet length
                packet += request->getFragment(DTPA::REQUEST_MAX_SIZE - packet.length(), &requestEnd);

                //If the request doesn't contain forms anymore
                if(requestEnd)
                {
                    /* If the request is completely ended (if the request had got all forms
                     * completed), remove it and reset the IDs array */
                    if(request->hasBeenCompleted())
                    {
                        //If the request needs an answer or is an answer, don't set the requestID as free
                        int requestID = request->getID();
                        if(!_funcAnswerRequest.contains(requestID) && request->getAnswerToID() == -1)
                            _requestIDs[requestID] = false;

                        delete _requestQueue.at(requestIndex);
                        _requestQueue.removeAt(requestIndex);

                        break;
                    }

                    //If there's a func to call when the request is ended, call it
                    callRequestEnd(request);
                }

            }while(packet.length() < DTPA::REQUEST_MAX_SIZE - 1);

        }while(packet.length() < DTPA::REQUEST_MAX_SIZE - 1 && _requestQueue.length() > 0);
    }

    return packet;
}

//If there's a func to call when the request is ended, call it
void DTPASender::callRequestAnswer(DTPARequest *request)
{
    int answerToID = request->getAnswerToID();
    funcRequestAnswer func = _funcAnswerRequest.value(answerToID, nullptr);
    if(func != nullptr)
    {
        int requestID = request->getID();

        //Call the function
        func(request, this, answerToID);

        if(request->isLastAnswer() && request->isComplete())
        {
            //Remove the request from the map
            _funcAnswerRequest.remove(answerToID);

            _requestIDs[requestID] = false;
        }
    }
}

//If there's a func to call when the request is ended, call it
void DTPASender::callRequestEnd(DTPARequest *request)
{
    int requestID = request->getID();
    funcRequestEnd func = _funcEndRequest.value(requestID, nullptr);
    if(func != nullptr)
    {
        //Remove the request from the map
        _funcEndRequest.remove(requestID);
        //Call the function
        func(request, this);
    }
}

//Send requestes in the queue as fragments starting from the one with higher priority
void DTPASender::sendPacket()
{
    QString packet = getPacketFromQueue();

    //Send packet with socket
    _socket->write(packet.toUtf8());
}

//Get the highest priority request
DTPARequest *DTPASender::getHighestPriorityRequest(int *outRequestIndex)
{

    DTPARequest *request = nullptr;

    //Get the highest priority request
    for(int i = 0; i < _requestQueue.length(); i++)
    {
        //If it's the first request or finds a request with higher priority than the currently selected
        if(!request || _requestQueue.at(i)->getPriority() > request->getPriority())
        {
            request = _requestQueue.at(i);
            *outRequestIndex = i;

            //If the request has the highest priority, don't continue
            if(request->getPriority() == DTPARequest::Priority::HIGHEST)
                break;
        }
    }

    return request;
}

//Get a valid id for the request
int16_t DTPASender::getValidRequestID()
{
    int16_t validID = 0;

    //Get a random id until the id in the array isn't used
    while(_requestIDs[validID])
    {
        validID++;      //from 0 to 9999
    }

    _requestIDs[validID] = true;

    return validID;
}

void DTPASender::freeAnswerID(DTPARequest *request, bool *deleteRequest)
{
    if(request->isComplete())
    {
        *deleteRequest = true;

        int reqID = request->getForms().first().getValue().toInt();
        _requestIDs[reqID] = false;
    }
}

//Get how many requests there are in the queue
int DTPASender::getQueueLength()
{
    return _requestQueue.length();
}

//Get the socket of the sender
QSslSocket *DTPASender::getSocket()
{
    return _socket;
}
