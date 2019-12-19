#pragma once

class DTPAForm
{
public:
    DTPAForm(QString value = "", bool isComplete = true);

    void setValue(QString value);
    QString getValue() const;

    QString getByteStuffedValue(int16_t stringMaxSize);

    void setComplete(bool isComplete);
    bool isComplete() const;

private:
    QString _value;

    bool _isComplete = false;
};

