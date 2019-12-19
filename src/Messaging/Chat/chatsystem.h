#pragma once
#include "src/Network/DataTransferProtocolAsh/dtparequest.h"
#include "src/Network/DataTransferProtocolAsh/dtpasender.h"

namespace ChatSystem
{
void initialize();

QString createChat(int userCreatorID);
void createPrivateChat(DTPASender *sender, DTPARequest *request, bool *deleteRequest);
void addUserToChat(int chatID, int userID);
};

