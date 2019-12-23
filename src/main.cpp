#include <QCoreApplication>

#include "Network/SSL/sslserver.h"

#include "Database/database.h"

#include "Messaging/User/userauthentication.h"
#include "Messaging/User/searchchatuser.h"
#include "Messaging/Chat/chatsystem.h"

#include "Other/config.h"
#include "Other/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Config::initialize();

    Logger::initialize();

    Database::initialize();
    UserAuthentication::initialize();
    ChatSystem::initialize();
    SearchChatUser::initialize();

    SslServer tcpServer;
    tcpServer.start();

    return a.exec();
}
