#pragma once
#include <QList>
#include <QDebug>

#include "dtpaform.h"

#define FRAGMENT_ID_SIZE 4         //Fragment id size in bytes (from 0 to 9999 in this case)
#define FRAGMENT_COMMAND_SIZE 4         //Fragment command size in bytes (from 0 to 9999 in this case)

class DTPARequest
{
public:
    enum class Command
    {
        NOTHING,                  //DO NOTHING (FOR TESTING)

        REGISTER_USER_EMAIL,      //AUTHENTICATION
        LOGIN_USER_EMAIL,
        LOGIN_USER_EMAIL_RESULT,
        REGISTER_USER_EMAIL_RESULT,

        CREATE_PRIVATE_CHAT,                 //CHAT
        NEW_PRIVATE_CHAT,

        GET_USER_INFO,
        GET_USER_INFO_RESULT,

        SEARCH_USERNAME,                //SEARCH AN USERNAME
        SEARCH_USERNAME_RESULT
    };

private:
    //How many forms each command need
    QMap<Command, int> _commandForms =
    {
        {Command::NOTHING, 0},      //DO NOTHING (FOR TESTING)

        {Command::REGISTER_USER_EMAIL, 2},      //AUTHENTICATION
        {Command::LOGIN_USER_EMAIL, 2},
        {Command::LOGIN_USER_EMAIL_RESULT, 1},
        {Command::REGISTER_USER_EMAIL_RESULT, 1},

        {Command::CREATE_PRIVATE_CHAT, 1},               //CHAT
        {Command::NEW_PRIVATE_CHAT, 2},

        {Command::GET_USER_INFO, 1},
        {Command::GET_USER_INFO_RESULT, 4},

        {Command::SEARCH_USERNAME, 1},
        {Command::SEARCH_USERNAME_RESULT, 1}
    };

public:

    enum class Priority
    {
        LOWEST,
        VERY_LOW,
        LOW,
        MEDIUM,
        HIGH,
        VERY_HIGH,
        HIGHEST
    };

    DTPARequest(Command command, QList<DTPAForm> forms, Priority priority = Priority::LOWEST, int16_t id = -1);
    DTPARequest(Command command, QStringList stringForms, Priority priority = Priority::LOWEST, int16_t id = -1);
    DTPARequest(Command command, QString stringForm, Priority priority = Priority::LOWEST, int16_t id = -1);
    DTPARequest(Command command, Priority priority = Priority::LOWEST, int16_t id = -1);

    void setup(Command command, QList<DTPAForm> forms, Priority priority, int16_t id);

    void answerToID(int16_t id, bool deleteAnswer);
    void answerTo(DTPARequest *request, bool deleteAnswer);
    int16_t getAnswerToID();
    bool isLastAnswer();

    void setID(int16_t id);
    int16_t getID();
    QString getStringID();

    Command getCommand();
    void setCommand(Command command);
    QString getStringCommand();

    void setForms(QList<DTPAForm> forms);
    void addNewForm(QString *stringForm, bool isFormComplete = true);
    void addForms(QList<DTPAForm> forms, bool isFormComplete);
    QList<DTPAForm> getForms();

    Priority getPriority() const;
    void setPriority(Priority priority);

    DTPARequest operator +(DTPARequest const &obj);


    QString getFragment(int maxFragmentSize, bool *outRequestEnd);

    bool isComplete();

private:
    int _totalContainedFormsCount = 0;
    bool _hasBeenCompleted = false;

    void setBeenCompleted();
public:
    bool hasBeenCompleted();

private:
    int16_t _id;
    int16_t _answerToID = -1;   //-1 means doesn't answer to anything
    bool _lastAnswer;
    Command _command;
    QList<DTPAForm> _forms;
    Priority _priority;

    //Get a string with a fixed length.. if the string is smaller than the fixed length, from left, available space will be filled with "0"
    QString getFixedLengthString(QString string, int fixedLength);
};

