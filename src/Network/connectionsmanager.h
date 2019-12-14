#pragma once
#include <QSslSocket>

namespace ConnectionsManager
{
static QMap<QSslSocket *, int> _socketToUserID;

static constexpr int USER_ID_NOT_FOUND = -1;

void addConnection(QSslSocket * socket, int userID);
void removeConnection(QSslSocket * socket);

int getUserID(QSslSocket * socket);
};

