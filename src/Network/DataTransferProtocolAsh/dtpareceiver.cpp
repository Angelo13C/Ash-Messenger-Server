#include "dtpareceiver.h"

//Setup the receiver
DTPAReceiver::DTPAReceiver(DTPASender *connection)
    : _connection(connection)
{

}

//Add a request to the vector of cached requests (requests that aren't ended but are started)
void DTPAReceiver::addToCachedRequests(DTPARequest request)
{
    bool alreadyCachedRequest = false;
    int i = 0;
    for(; i < _cachedRequests.length(); i++)
    {
        if(_cachedRequests[i].getID() == request.getID())
        {
            alreadyCachedRequest = true;
            _cachedRequests[i] = _cachedRequests[i] + request;
            break;
        }
    }

    //If the request is new
    if(!alreadyCachedRequest)
        _cachedRequests.append(request);

    //Call the function that needs to be answered
    _connection->callRequestAnswer(&request);

    bool deleteRequest = false;
    DTPAReceiverManager::getInstance()->callRequestCommand(_connection, &_cachedRequests[i], &deleteRequest);

    //If the command says that the request needs to be deleted, delete it
    if(deleteRequest)
        removeFromCachedRequests(&_cachedRequests[i]);
}

//Remove a request from the vector of cached requests
void DTPAReceiver::removeFromCachedRequests(DTPARequest *request)
{
    int requestID = request->getID();
    for(int i = 0; i < _cachedRequests.length(); i++)
    {
        if(_cachedRequests[i].getID() == requestID)
        {
            _cachedRequests.removeAt(i);
            break;
        }
    }
}

//Get requests from a packet
QList<DTPARequest> DTPAReceiver::packetToRequests(QString packet)
{
    QList<DTPARequest> requests;
    //Get the fragments of the packet
    QStringList fragments = DTPA::packetToFragments(packet);

    foreach (QString fragment, fragments) {
        requests.append(fragmentToRequest(fragment));
    }

    return  requests;
}

//Decode a fragment and get the request
DTPARequest DTPAReceiver::fragmentToRequest(QString fragment)
{
    int16_t fragmentID;
    int16_t answerToID = -1;
    bool lastAnswer;
    DTPARequest::Command fragmentCommand;
    QList<DTPAForm> forms;

    //Get the fragment's id
    fragmentID = fragment.mid(DTPA::TOKEN_FRAGMENT_DELIMITER.length(), TOKEN_FRAGMENT_ID_SIZE).toShort();
    fragment.remove(0, TOKEN_FRAGMENT_ID_SIZE + DTPA::TOKEN_FRAGMENT_DELIMITER.length());

    //Get the fragment's command
    fragmentCommand = static_cast<DTPARequest::Command>(fragment.mid(0, TOKEN_CMDTKN_SIZE).toShort());
    fragment.remove(0, TOKEN_CMDTKN_SIZE);

    //If the request answers to something
    if(fragment.at(0) != 'N')
    {
        answerToID = fragment.mid(0, TOKEN_FRAGMENT_ID_SIZE).toShort();
        fragment.remove(0, TOKEN_FRAGMENT_ID_SIZE);

        lastAnswer = fragment.at(0) == 'Y';
        //The 'Y' or 'N' is 1 char
        fragment.remove(0, 1);
    }
    else
    {
        //Remove the 'N' char
        fragment.remove(0, 1);
    }

    //Get the forms of the fragment (that are encoded with '[' between fragment)
    QString encodedForms = fragment;
    encodedForms.chop(1);
    //Split the fragments
    bool isLastFormCompleted;
    QStringList stringFormsList = DTPA::separateForms(encodedForms, &isLastFormCompleted);

    //Byte stuff every form in the list
    for(int i = 0; i < stringFormsList.length(); i++)
        DTPA::byteStuff(&stringFormsList[i], true);

    //Add the splitted forms to the forms list
    for(int i = 0; i < stringFormsList.length(); i++)
    {
        bool isFormComplete = i < stringFormsList.length() - 1 ? true : isLastFormCompleted;
        DTPAForm form(stringFormsList.at(i), isFormComplete);

        forms.append(form);
    }

    DTPARequest request(fragmentCommand, forms);
    request.setID(fragmentID);

    //If it is an answer
    if(answerToID != -1)
        request.answerToID(answerToID, lastAnswer);

    return request;
}

