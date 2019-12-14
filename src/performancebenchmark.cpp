#include "performancebenchmark.h"

PerformanceBenchmark::PerformanceBenchmark(QString name):
    _name(name)
{
    _timer.start();
}

PerformanceBenchmark::~PerformanceBenchmark()
{
    qDebug() << "Benchmark " << _name << ": " << _timer.elapsed() << " ms";
}
