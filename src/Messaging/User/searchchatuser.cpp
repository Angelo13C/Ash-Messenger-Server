#include "searchchatuser.h"

//Initialize the namespace
void SearchChatUser::initialize()
{
    DTPAReceiverManager::getInstance()->signCommandFunction
            (DTPARequest::Command::SEARCH_USERNAME, searchUsername);
}

void SearchChatUser::searchUsername(DTPASender *sender, DTPARequest *request, bool *deleteRequest)
{
    if(request->isComplete())
    {
        *deleteRequest = true;

        QString username = request->getForms().first().getValue();

        QVector<QStringList> sameUsernameID = Database::execute("SELECT user_id, username FROM users WHERE LOWER(username) = "
                                                "LOWER('" + username + "');");

        if(sameUsernameID.size() > 0)
        {
            QString sameUserID = sameUsernameID.at(0).at(0);

            DTPARequest req = DTPARequest(DTPARequest::Command::SEARCH_USERNAME_RESULT,
                                            sameUserID, DTPARequest::Priority::HIGH);
            sender->sendRequest(&req);
        }

        QString sameUsername;
        if(sameUsernameID.size() > 0)
            sameUsername = sameUsernameID.at(0).at(1);

        /*Get max 5 (- 1 if has found a user with the same username) rows
         * that have the username that starts with the searched username */
        QVector<QStringList> result = Database::execute("SELECT user_id FROM users WHERE username ILIKE "
                "'" + username + "%' AND username <> '" + sameUsername + "' FETCH FIRST "
                + QString::number(5 - sameUsernameID.size()) + " ROWS ONLY;");

        for(int i = 0; i < result.size(); i++)
        {
            QString userID = result.at(i).at(0);

            DTPARequest req = DTPARequest(DTPARequest::Command::SEARCH_USERNAME_RESULT,
                                          userID, DTPARequest::Priority::HIGH);

            sender->sendRequest(&req);
        }
    }
}
