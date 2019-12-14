#pragma once
#include "src/Network/DataTransferProtocolAsh/dtpareceivermanager.h"

#include "src/Network/connectionsmanager.h"

#include "src/Database/database.h"

namespace ChatSystem
{
void initialize();

QString createChat(int userCreatorID);
void createPrivateChat(DTPASender *sender, DTPARequest *request, bool *deleteRequest);
void addUserToChat(int chatID, int userID);
};

