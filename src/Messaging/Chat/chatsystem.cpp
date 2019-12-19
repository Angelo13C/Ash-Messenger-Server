#include "chatsystem.h"

#include "src/Network/DataTransferProtocolAsh/dtpareceivermanager.h"

#include "src/Network/connectionsmanager.h"

#include "src/Database/database.h"

void ChatSystem::initialize()
{
    DTPAReceiverManager::getInstance()->signCommandFunction(DTPARequest::Command::CREATE_PRIVATE_CHAT, createPrivateChat);
}

QString ChatSystem::createChat(int userCreatorID)
{

    QString chatID = Database::execute("INSERT INTO chats DEFAULT VALUES RETURNING chat_id;").at(0).at(0);

    addUserToChat(chatID.toInt(), userCreatorID);        //Add the user that created the chat to the chat

    return chatID;
}

//Add a user to a chat
void ChatSystem::addUserToChat(int chatID, int userID)
{
    //Create a new row of the table and add the user
    Database::execute("INSERT INTO users_in_chats(user_id, chat_id) VALUES (" +
                      QString::number(userID) + "," + QString::number(chatID) + ");");
}

//Create a private chat between 2 users
void ChatSystem::createPrivateChat(DTPASender *sender, DTPARequest *request, bool *deleteRequest)
{
    if(request->isComplete())
    {
        *deleteRequest = true;

        //Get the userID of the chat creator
        int userID = ConnectionsManager::getUserID(sender->getSocket());

        QString chatID = createChat(userID);

        //Add the other user of the private chat to the chat
        QString otherUserID = request->getForms().first().getValue();
        addUserToChat(chatID.toInt(), otherUserID.toInt());

        //Send the chat id to the user
        DTPARequest request = DTPARequest(DTPARequest::Command::NEW_PRIVATE_CHAT,
                    QStringList() << chatID << otherUserID, DTPARequest::Priority::HIGH);
        sender->sendRequest(&request);
    }
}
