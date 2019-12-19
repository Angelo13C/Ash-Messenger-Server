#pragma once

#include <QTime>

class PerformanceBenchmark
{
public:
    PerformanceBenchmark(QString name);
    ~PerformanceBenchmark();

private:
    QString _name;
    QTime _timer;
};

