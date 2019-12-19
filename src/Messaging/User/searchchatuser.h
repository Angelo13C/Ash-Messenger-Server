#pragma once

#include "src/Network/DataTransferProtocolAsh/dtparequest.h"
#include "src/Network/DataTransferProtocolAsh/dtpasender.h"

namespace SearchChatUser
{
void initialize();

void searchUsername(DTPASender *sender, DTPARequest *request, bool *deleteRequest);
};

