#include "config.h"

//Initialize the config reading the config.json
void Config::initialize()
{
    QFile configFile(":/CONFIG/resources/Config/config.json");
    configFile.open(QIODevice::ReadOnly);

    _configDoc = QJsonDocument::fromJson(configFile.readAll());

    configFile.close();
}

//Read the value associated to the json file with that key
QString Config::readValue(QString rawKey)
{
    //Separate keys by /
    QStringList separatedKeys = rawKey.split("/");

    QJsonObject valuesObject = _configDoc.object();

    //Go through all the objects with that key
    for(int i = 0; i < separatedKeys.length() - 1; i++) {
        valuesObject = valuesObject[separatedKeys.at(i)].toObject();
    }

    //Get the value of the last part of the raw key
    return valuesObject[separatedKeys.last()].toString();
}
