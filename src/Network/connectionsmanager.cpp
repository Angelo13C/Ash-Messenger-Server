#include "connectionsmanager.h"


//Add a socket to the manager and his id
void ConnectionsManager::addConnection(QSslSocket *socket, int userID)
{
    _socketToUserID.insert(socket, userID);
}

//Remove a socket when the user ends the connection
void ConnectionsManager::removeConnection(QSslSocket *socket)
{
    _socketToUserID.remove(socket);
}

//Get the userID of the socket
int ConnectionsManager::getUserID(QSslSocket *socket)
{
    return _socketToUserID.value(socket, USER_ID_NOT_FOUND);
}
