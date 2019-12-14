#pragma once
#include <QVector>
#include <QStringList>

#include "src/Database/database.h"

#include "src/Network/DataTransferProtocolAsh/dtpareceivermanager.h"
#include "src/Network/DataTransferProtocolAsh/dtpasender.h"

namespace SearchChatUser
{
void initialize();

void searchUsername(DTPASender *sender, DTPARequest *request, bool *deleteRequest);
};

