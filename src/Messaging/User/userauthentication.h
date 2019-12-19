#pragma once

#include "src/Network/DataTransferProtocolAsh/dtpasender.h"
#include "src/Network/DataTransferProtocolAsh/dtparequest.h"

namespace UserAuthentication
{
void initialize();

void registerEmail(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
void loginEmail(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
void getUserInfo(DTPASender * sender, DTPARequest * request, bool *deleteRequest);
};

