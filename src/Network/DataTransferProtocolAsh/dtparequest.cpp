#include "dtparequest.h"

#include "dtpa.h"

//Setup the request with all the values
void DTPARequest::setup(Command command, QList<DTPAForm> forms, Priority priority, int16_t id)
{
    setCommand(command);
    setForms(forms);
    setPriority(priority);

    if(id != -1)
        setID(id);
}

DTPARequest::DTPARequest(Command command, QList<DTPAForm> forms, Priority priority, int16_t id)
{
    setup(command, forms, priority, id);
}

//If there aren't forms
DTPARequest::DTPARequest(Command command, Priority priority, int16_t id)
{
    setup(command, QList<DTPAForm>(), priority, id);
}


DTPARequest::DTPARequest(DTPARequest::Command command, QStringList stringForms, DTPARequest::Priority priority, int16_t id)
{
    //Create the forms from the string list
    QList<DTPAForm> forms = QList<DTPAForm>();

    for(int i = 0; i < stringForms.length(); i++)
    {
        forms.append(DTPAForm(stringForms.at(i)));
    }

    setup(command, forms, priority, id);
}

DTPARequest::DTPARequest(DTPARequest::Command command, QString stringForm, DTPARequest::Priority priority, int16_t id)
{
    //Create the forms from the string list
    QList<DTPAForm> forms = QList<DTPAForm>();
    forms.append(DTPAForm(stringForm));

    setup(command, forms, priority, id);
}

//Set the answer to the id of the request
void DTPARequest::answerToID(int16_t id, bool deleteAnswer)
{
    _answerToID = id;
    _lastAnswer = deleteAnswer;
}

//Set the answer to the id of the request
void DTPARequest::answerTo(DTPARequest *request, bool deleteAnswer)
{
    _answerToID = request->getID();
    _lastAnswer = deleteAnswer;
}

//Get the answer to the id of the request
int16_t DTPARequest::getAnswerToID()
{
    return _answerToID;
}

//Is the last request of an answer
bool DTPARequest::isLastAnswer()
{
    return _lastAnswer;
}

//Get a string with a fixed length in bytes.. if the string is smaller than the fixed length, from left, available space will be filled with "0"
QString DTPARequest::getFixedLengthString(QString string, int fixedLength)
{
    //Return has a fixed length.. if string is "560", return value will be "00000560"
    return QString("0").repeated(fixedLength - string.length()) + string;
}

//Set the request's id
void DTPARequest::setID(int16_t id)
{
    _id = id;
}

//Get request's id
int16_t DTPARequest::getID()
{
    return _id;
}

//Get request's id as string with fixed length
QString DTPARequest::getStringID()
{
    return getFixedLengthString(QString::number(_id), FRAGMENT_COMMAND_SIZE);
}

//Set the request's id
void DTPARequest::setCommand(Command command)
{
    _command = command;
}

//Get request's id
DTPARequest::Command DTPARequest::getCommand()
{
    return _command;
}

//Get request's command as string with fixed length
QString DTPARequest::getStringCommand()
{
    return getFixedLengthString(QString::number(static_cast<int>(_command)), FRAGMENT_COMMAND_SIZE);
}

//Set the request's forms
void DTPARequest::setForms(QList<DTPAForm> forms)
{
    _totalContainedFormsCount = forms.length();

    _forms = forms;

    //If the request hasn't been completed, check if it is
    setBeenCompleted();
}

//Add a new form to the request
void DTPARequest::addNewForm(QString *stringForm, bool isFormComplete)
{
    _forms.append(DTPAForm(*stringForm, isFormComplete));

    _totalContainedFormsCount++;
    //If the request hasn't been completed, check if it is
    setBeenCompleted();
}

//Add forms to the request
void DTPARequest::addForms(QList<DTPAForm> forms, bool isFormComplete)
{
    //If the last form of the request isn't complete, add more contents to it
    if(!_forms.isEmpty())
    {
        DTPAForm *lastForm = &_forms.last();
        if(!lastForm->isComplete())
        {
            //Set the last current form to complete if the first form of the new forms is complete
            lastForm->setComplete(forms.first().isComplete());

            lastForm->setValue(lastForm->getValue() + forms.first().getValue());
            forms.removeFirst();
        }
    }

    _totalContainedFormsCount += forms.length();
    _forms.append(forms);
    //If there are more forms than the needed by the command of the request
    if(_forms.length() > _commandForms.value(_command))
    {
        qDebug() << "WARNING: Form overflow!";
    }

    _forms.last().setComplete(isFormComplete);

    //If the request hasn't been completed, check if it is
    setBeenCompleted();
}

//Get request's forms
QList<DTPAForm> DTPARequest::getForms()
{
    return _forms;
}

//Set the requests's priority
void DTPARequest::setPriority(DTPARequest::Priority priority)
{
    _priority = priority;
}

//Get the requests's priority
DTPARequest::Priority DTPARequest::getPriority() const
{
    return  _priority;
}

//Merge 2 requests
DTPARequest DTPARequest::operator +(const DTPARequest &obj)
{
    //If there are forms in the other request
    if(!obj._forms.empty())
        this->addForms(obj._forms, _forms.isEmpty() ? true : obj._forms.last().isComplete());

    return *this;
}

//Get the first fragment of the request and then deletes it (to pass through all fragments of the request)
QString DTPARequest::getFragment(int maxFragmentSize, bool *outRequestEnd)
{
    QString fragment;

    //Add the fragment's id and command stringed
    fragment = getStringID();
    fragment += getStringCommand();

    //If the request answers to a request, add the id of the request as answer, else N
    if(getAnswerToID() != -1)
    {
        fragment += getFixedLengthString(QString::number(getAnswerToID()), 4);
        fragment += _lastAnswer ? "Y" : "N";
    }
    else
        fragment += "N";

    //Add the fragment's forms
    //Get the  list of forms of a request
    QString formString;

    //Pass through the forms of the request and get their values
    while(!_forms.isEmpty())
    {
        formString = _forms.first().getByteStuffedValue(static_cast<int16_t>(maxFragmentSize - fragment.length() - (DTPA::TOKEN_FRAGMENT_DELIMITER.length() * 2)));
        fragment += formString;

        /*  If the fragment is longer than the max size minus the 3 delimiters
            (fragment start, fragment end, form delimiter)
            and there is at least another form in the request stops adding forms and form delimiters  */
        if(fragment.length() >= maxFragmentSize - 3 && _forms.first().getValue().length() > 0)
            break;

        //Delete the fragment from the list if it's completed
        if(_forms.first().isComplete())
        {
            fragment += DTPA::TOKEN_FORMS_DELIMITER;
            _forms.removeFirst();
        }
        else
            break;
    }

    //Add the fragment's delimiters at the start and at the end of the fragment
    fragment = DTPA::TOKEN_FRAGMENT_DELIMITER + fragment;
    fragment += DTPA::TOKEN_FRAGMENT_DELIMITER;

    //If the request is empty or it has got a form that's not completed but empty
    if(_forms.isEmpty() || (_forms.size() == 1 && _forms.first().getValue() == ""))
        *outRequestEnd = true;
    else
        *outRequestEnd = false;

    return fragment;
}

//Is the request complete of its' forms?
bool DTPARequest::isComplete()
{
    //If the count of forms is the same as defined in command forms and the last form is completed
    return _forms.length() >= _commandForms.value(_command) && _forms.last().isComplete();
}

void DTPARequest::setBeenCompleted()
{
    //If the request hasn't been completed, check if it is
    if(!_hasBeenCompleted)
    {
        if(_forms.size() > 0)
        {
            _hasBeenCompleted = _totalContainedFormsCount >= _commandForms.value(_command)
                    && _forms.last().isComplete();
        }
        else
        {
            _hasBeenCompleted = _totalContainedFormsCount >= _commandForms.value(_command);
        }
    }
}

//If the request is ever been completed
bool DTPARequest::hasBeenCompleted()
{
    return _hasBeenCompleted;
}
