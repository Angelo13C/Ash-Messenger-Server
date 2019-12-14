#include <QCoreApplication>

#include "Network/SSL/sslserver.h"
#include "Database/database.h"
#include "src/Messaging/User/userauthentication.h"
#include "src/Other/config.h"
#include "src/Messaging/Chat/chatsystem.h"
#include "src/Messaging/User/searchchatuser.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Config::initialize();

    Database::initialize();
    UserAuthentication::initialize();
    ChatSystem::initialize();
    SearchChatUser::initialize();

    SslServer tcpServer;
    tcpServer.start();

    return a.exec();
}
