#pragma once

#include <QJsonDocument>

namespace Config
{
static QJsonDocument _configDoc;

void initialize();

QString readValue(QString rawKey);
};

