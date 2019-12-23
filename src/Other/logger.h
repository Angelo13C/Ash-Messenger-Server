#pragma once

namespace Logger
{
static QFile *_logFile;
static QTextStream *_logStream;
static QString _timeFormat;

void initialize();
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString & msg);
};

