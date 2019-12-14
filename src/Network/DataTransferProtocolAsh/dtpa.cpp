#include "dtpa.h"

//If the string is smaller than the fixed length, from left, available space will be filled with "0"
QString DTPA::getFixedLengthString(QString string, int fixedLength)
{   //i.e. if the string is "560", the return value will be "00000560"
    return QString("0").repeated(fixedLength - string.length()) + string;
}

//Byte stuff (or undo it) a string
void DTPA::byteStuff(QString *string, bool undo)
{
    if(!undo)
    {
        string->replace(BYTE_STUFFER_CHAR, BYTE_STUFFER_CHAR + BYTE_STUFFER_CHAR);
        string->replace(TOKEN_FRAGMENT_DELIMITER, BYTE_STUFFER_CHAR + TOKEN_FRAGMENT_DELIMITER);
        string->replace(TOKEN_FORMS_DELIMITER, BYTE_STUFFER_CHAR + TOKEN_FORMS_DELIMITER);
    }
    else
        string->replace(BYTE_STUFFER_CHAR + BYTE_STUFFER_CHAR, BYTE_STUFFER_CHAR);
}

//Pass a string with a delimiter character (~, [) and its position and the function will return if the character it's used as a delimiter or not
bool DTPA::isByteStuffed(QString string, int position)
{
    int index = position;
    int numberOfNewLines = 0;

    //The function counts how many \n (newlines) there are attached before the "delimiter"
    while(index > 0 && string.at(index - 1) == '\n')
    {
        index--;
        numberOfNewLines++;
    }

    return numberOfNewLines % 2;
}

//Find the token delimiters on a string and call a separateAction (to split a string by delimiters)
QStringList DTPA::separateDelimiters(QString *string, QString tokenDelimiter, SeparateAction separateAction, bool *noDelimiterFound)
{

    /*  Do this because if in the form there is a token_fragment_delimiter that doesn't need
        to delimite forms, and you split the string after the bytestuffing,
        you can't know which of these delimiters where wanted */

    int delimiterIndex = 0;
    QStringList stringList;

    while(true)
    {
        //Get the index of the first token delimiter available from index position of string
        delimiterIndex = string->indexOf(tokenDelimiter, delimiterIndex);

        if (delimiterIndex == -1)    //If there aren't tokenDelimiters available, the loop ends
            break;

        separateAction(string, &delimiterIndex, &stringList);
    }

    //If no delimiter is found
    if(stringList.isEmpty())
    {
        if(*string != "")
        {
            stringList.append(*string);
            *string = "";
        }

        if(noDelimiterFound != nullptr)
            *noDelimiterFound = true;
    }
    else if(noDelimiterFound != nullptr)
        *noDelimiterFound = false;

    return stringList;
}

//Get from a packet the contained fragments in a list
QStringList DTPA::packetToFragments(QString packet)
{
    /* When the separateDelimiters finds the ~ delimiter, if it's at the start of the fragment
       it needs only to delete the ~, else he knows that he has found the end of the fragment */
    bool fragmentStarted = false;

    QStringList fragments = separateDelimiters(&packet, TOKEN_FRAGMENT_DELIMITER,
        [&fragmentStarted](QString *string, int *delimiterIndex, QStringList *stringList)
        {
            //If the found form is a delimiter or is a character send as data
            if(!isByteStuffed(string->mid(0, *delimiterIndex), *delimiterIndex))
            {
                fragmentStarted = !fragmentStarted;

                if(!fragmentStarted)    //If the found delimiter is the end of the fragment
                {
                    stringList->append(string->mid(0, *delimiterIndex + 1));
                    string->remove(0, *delimiterIndex + 1);

                    *delimiterIndex = 0;
                }
                else    //If the found delimiter is the start of the fragment
                {
                    ++*delimiterIndex;
                }
            }
            //If the delimiter is data, remove the \n before it (byte stuffing)
            else
            {
                string->remove(*delimiterIndex - 1, 1);
            }
        }
    );

    return fragments;
}

//Get from a fragment's data the forms in a list
QStringList DTPA::separateForms(QString fragmentData, bool *outIsLastFormCompleted)
{
    bool noFormDelimiterFound;
    //Get separated forms by TOKEN_FORMS_DELIMITER character
    QStringList forms = separateDelimiters(&fragmentData, TOKEN_FORMS_DELIMITER,
        [](QString *string, int *delimiterIndex, QStringList *stringList)
        {
            //If it's a form delimiter, add the string before the delimiter to the list and remove the delimiter
            if (!isByteStuffed(*string, *delimiterIndex))
            {
                stringList->append(string->mid(0, *delimiterIndex));
                string->remove(0, *delimiterIndex + 1);
                *delimiterIndex = 0;
            }
            else
            {
                string->remove(*delimiterIndex - 1, 1);
            }
        }
    , &noFormDelimiterFound);

    if(fragmentData != "")
        forms.append(fragmentData);

    //If last part of the fragmentData is empty it means that the form is completed
    if (outIsLastFormCompleted)
        *outIsLastFormCompleted = fragmentData == "" && !noFormDelimiterFound;

    return forms;
}
