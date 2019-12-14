#pragma once
#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace Config
{
static QJsonDocument _configDoc;

void initialize();

QString readValue(QString rawKey);
};

