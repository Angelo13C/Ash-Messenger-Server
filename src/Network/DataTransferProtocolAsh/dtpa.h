#pragma once

#include <functional>

#include <QVector>
#include <QList>
#include <QString>
#include <QStringList>

#define TOKEN_FRAGMENT_ID_SIZE 4       //Bytes (fragment's id token)
#define TOKEN_CMDTKN_SIZE 4       //Bytes (command token)
#define TOKEN_DATA_SIZE 4096 - 2 - TOKEN_CMDTKN_SIZE - TOKEN_FRAGMENT_ID_SIZE    //Bytes (data token) (2 is the size of the fragment_flag start and end)

namespace DTPA
{
QString getFixedLengthString(QString string, int fixedLength);

static const QString BYTE_STUFFER_CHAR = "\n";             //Char that's used to byte stuff
static const QString TOKEN_FRAGMENT_DELIMITER = "~";          //Char that indicates the start/end of a fragment
static const QString TOKEN_FORMS_DELIMITER = "[";          //Char that delimits forms

static constexpr int REQUEST_MAX_SIZE = 4096;            //Max size of a ssl packet

void byteStuff(QString *string, bool undo = false);
bool isByteStuffed(QString string, int position);

typedef const std::function<void(QString*, int*, QStringList*)> &SeparateAction;
QStringList separateDelimiters(QString *string, QString tokenDelimiter, SeparateAction separateAction, bool *noDelimiterFound = nullptr);

QStringList packetToFragments(QString packet);
QStringList separateForms(QString fragmentData, bool *outIsLastFormCompleted = nullptr);
};
