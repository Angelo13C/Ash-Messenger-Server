#pragma once
#include <iostream>

#include "src/Network/DataTransferProtocolAsh/dtpa.h"
#include "src/Network/DataTransferProtocolAsh/dtpareceivermanager.h"

#include <QStringList>
#include <QString>
#include "src/Database/database.h"

#include "src/Network/connectionsmanager.h"

namespace UserAuthentication
{
void initialize();

void registerEmail(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
void loginEmail(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
void getUserInfo(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
};

