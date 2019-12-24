#include "logger.h"

#include <QDateTime>

#include "src/Other/config.h"

//Initialize the logger
void Logger::initialize()
{
    _logFile = new QFile(Config::readValue("logger/path"));
    _logFile->open(QIODevice::WriteOnly | QIODevice::Append);

    _logStream = new QTextStream(_logFile);
    *_logStream << endl;

    _timeFormat = Config::readValue("logger/timeFormat");

    qInstallMessageHandler(Logger::messageHandler);
}

//When qDebug or other log functions are called
void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString & msg)
{
    Q_UNUSED(context)

    QString logText = QDateTime::currentDateTime().toString(_timeFormat);

    if(type == QtDebugMsg)
        logText += "Debug: ";
    else if(type == QtInfoMsg)
        logText += "Info: ";
    else if(type == QtWarningMsg)
        logText += "Warning: ";
    else if(type == QtCriticalMsg)
        logText += "Critical: ";
    else if(type == QtFatalMsg)
        logText += "Fatal: ";

    logText += msg;

    //Output to the file (append)
    *_logStream << logText << endl;

    //Output to the console
    qDebug().noquote() << logText;
}
