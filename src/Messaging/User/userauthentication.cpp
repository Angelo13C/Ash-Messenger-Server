#include "userauthentication.h"

#include "src/Network/DataTransferProtocolAsh/dtpareceivermanager.h"

#include "src/Database/database.h"

#include "src/Network/connectionsmanager.h"

void UserAuthentication::initialize()
{
    DTPAReceiverManager::getInstance()->signCommandFunction(DTPARequest::Command::REGISTER_USER_EMAIL, registerEmail);
    DTPAReceiverManager::getInstance()->signCommandFunction(DTPARequest::Command::LOGIN_USER_EMAIL, loginEmail);

    DTPAReceiverManager::getInstance()->signCommandFunction(DTPARequest::Command::GET_USER_INFO, getUserInfo);
}

#include <QRandomGenerator> //DELETE
//User tries to register (with email)
void UserAuthentication::registerEmail(DTPASender *sender, DTPARequest *request, bool *deleteRequest)
{
    if(request->isComplete())
    {
        //Delete the received request
        *deleteRequest = true;

        QString email = request->getForms().at(0).getValue();   //Email;

        QVector<QStringList> resExists = Database::executeBinded("SELECT EXISTS (SELECT 1 FROM users_email WHERE email = $1);", QStringList() << email);

        QString alreadyExists = resExists.at(0).at(0);
        DTPARequest registerResult(DTPARequest::Command::REGISTER_USER_EMAIL_RESULT, DTPARequest::Priority::HIGH);

        //If a user with the same email already exists
        if(alreadyExists == "t")
        {
            //If the registration is failed, put -1 as a result for the client
            QString result = "-1";
            registerResult.addNewForm(&result);
        }
        else
        {
            //Insert the real name here
            QString name = "random name";

            QStringList list{"a", "b", "c", "d", "e", "f", "g", "h"};       //TO DELETE
            std::random_device rd;       //TO DELETE
            std::mt19937 g(rd());       //TO DELETE
            std::shuffle(list.begin(), list.end(), g);       //TO DELETE
            QString randomString = list.join("");       //TO DELETE

            //Insert the real userName here
            QString userName = randomString;

            //If the username is null, put as null so that the database can understand
            QString userID = Database::executeBinded("INSERT INTO users (name, username) VALUES "
                       "($1, NULLIF($2,'')) RETURNING user_id;", QStringList() << name << userName).at(0).at(0);

            QStringList parameters = QStringList() << userID;
            parameters << email;
            parameters << request->getForms().at(1).getValue();   //Password
            parameters << "salt";

            Database::executeBinded("INSERT INTO users_email(user_id, email, password_hash,"
                                 "password_salt) VALUES ($1, $2, $3, $4);", parameters);

            registerResult.addNewForm(&userID);

            ConnectionsManager::addConnection(sender->getSocket(), userID.toInt());
        }
        //Send to the user the result of the register
        sender->sendRequest(&registerResult);
    }
}

//User tries to login (with email)
void UserAuthentication::loginEmail(DTPASender *sender, DTPARequest *request, bool *deleteRequest)
{
    //Get email and password sended by user
    if(request->isComplete())
    {
        //Delete the received request
        *deleteRequest = true;

        QString email = request->getForms().at(0).getValue();
        QString password = request->getForms().at(1).getValue();

        //Get the user with the specified email and password
        //QVector<QStringList> result = Database::executeBinded("SELECT * FROM users_email "
        //        "WHERE email = ? AND password_hash = ?;", QStringList() << email << password);

        QVector<QStringList> result;
        result.append(QStringList() << "301");
        DTPARequest loginResult(DTPARequest::Command::LOGIN_USER_EMAIL_RESULT, DTPARequest::Priority::HIGH);

        //If there is a user with that email and password
        if(result.size() > 0)
        {
            QString userID = result.at(0).at(0);
            loginResult.addNewForm(&userID);

            ConnectionsManager::addConnection(sender->getSocket(), userID.toInt());
        }
        else    //Login failed
        {
            QString result = "-1";
            //If the login is failed, put -1 as a result for the client
            loginResult.addNewForm(&result);
            qDebug() << "No user with that email and password found!";
        }
        sender->sendRequest(&loginResult);
    }
}

//Client needs basic info about the user with id
void UserAuthentication::getUserInfo(DTPASender *sender, DTPARequest *request, bool *deleteRequest)
{
    if(request->isComplete())
    {
        //Delete the received request
        *deleteRequest = true;

        QString userID = request->getForms().first().getValue();
        QVector<QStringList> result = Database::execute("SELECT name, username, pic_path FROM users WHERE user_id = " + userID);

        if(result.size() > 0)
        {
            QString name = result.at(0).at(0);
            QString username = result.at(0).at(1);
            QString picturePath = result.at(0).at(2);

            DTPARequest userInfoRequest(DTPARequest::Command::GET_USER_INFO_RESULT,
                    QStringList() << userID << name << username << picturePath, DTPARequest::Priority::HIGH);

            sender->sendRequest(&userInfoRequest);
        }
    }
}
