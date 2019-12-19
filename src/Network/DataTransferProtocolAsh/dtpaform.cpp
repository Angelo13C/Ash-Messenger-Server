#include "dtpaform.h"

#include "dtpa.h"

DTPAForm::DTPAForm(QString value, bool isComplete)
{
    setComplete(isComplete);
    setValue(value);
}

//Set the form's value
void DTPAForm::setValue(QString value)
{
    _value = value;
}

//Get the form's value
QString DTPAForm::getValue() const
{
    return _value;
}

/*Set the form to complete status or uncomplete
  (useful for seeing if a new form in a request belongs to this form)*/
void DTPAForm::setComplete(bool isComplete)
{
    _isComplete = isComplete;
}

//Get complete status
bool DTPAForm::isComplete() const
{
    return _isComplete;
}

//Get the form's value byte stuffed with a limit of characters
QString DTPAForm::getByteStuffedValue(int16_t stringMaxSize)
{
    if(_value.length() == 0)
        return "";

    //Set the stringToByteStuff to the substring with max length of 4096
    QString stringToByteStuff = _value.mid(0, stringMaxSize);

    //Get the string length and byte stuff the string
    int stringByteStuffedLength = stringToByteStuff.length();
    DTPA::byteStuff(&stringToByteStuff, false);

    //If the byteStuffed string is shorter than the max size, returns without doing other calculations
    if(stringToByteStuff.length() < stringMaxSize)
    {
        //-1 because the stringStartSize is the length so it's +1
        _value.remove(0, stringByteStuffedLength - (_value.length() != stringMaxSize - 1));

        return stringToByteStuff;
    }

    int returnLength;
    returnLength = stringToByteStuff.length() > stringMaxSize ? stringMaxSize : stringToByteStuff.length();
    returnLength -= stringToByteStuff.at(stringMaxSize - 1) == "\n" && !DTPA::isByteStuffed(stringToByteStuff, stringMaxSize - 1);

    /*Get how many characters the function needs to remove
      from the _value (without the byte stuffed chars) */
    QString stringToCompare = stringToByteStuff.mid(0, returnLength);
    DTPA::byteStuff(&stringToCompare,true);
    stringToCompare.replace("\n~", "~");
    stringToCompare.replace("\n[", "[");

    //Remove the right amount of characters
    _value.remove(0, stringToCompare.length());

    return stringToByteStuff.mid(0, returnLength);
}
