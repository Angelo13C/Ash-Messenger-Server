#include "sslconnection.h"

#include "src/Network/connectionsmanager.h"

#include <QSslKey>
#include <QHostAddress>

SslConnection::SslConnection(QObject *parent, int connectionPoolID, int connectionID)
    : QObject(parent), _connectionPoolID(connectionPoolID), _connectionID(connectionID),
      _logPrefix("Connection " + QString::number(connectionPoolID)
                 + "-" + QString::number(connectionID))
{

}

//Delete the socket... from the heap
SslConnection::~SslConnection()
{
    delete _socket;
    delete _sender;
    delete _receiver;
}

//Get the dtpa sender of the connection
DTPASender *SslConnection::getSender()
{
    return _sender;
}

int SslConnection::getID()
{
    return _connectionID;
}

//Get the current socket
QSslSocket *SslConnection::getSocket()
{
    return _socket;
}

//When a client connects
void SslConnection::onConnect()
{
    if(!sender())
        return;

    bool ok;
    QHostAddress address(_socket->peerAddress().toIPv4Address(&ok));

    qInfo().noquote() << _logPrefix << "connected";
    qInfo().noquote().nospace() << _logPrefix << " address: " <<
        address.toString() << ":" << _socket->peerPort();

    emit connected();   //Emit the signal to be handled by the pool
}

//When a client disconnects
void SslConnection::onDisconnect()
{
    if(!sender())
        return;
    qInfo().noquote() << _logPrefix << "disconnected";

    ConnectionsManager::removeConnection(_socket);
    emit disconnected();        //emit the signal to be handled by the pool
}

//When the connection receive a message
void SslConnection::onReadyRead()
{
    if(!sender())
        return;

    qInfo().noquote() << _logPrefix << "readyRead";

    QString packet = readSocket();

    if(packet != "")
    {
        QList<DTPARequest> requests = DTPAReceiver::packetToRequests(packet);

        for(int i = 0; i < requests.length(); i++)
        {
            DTPARequest request = DTPARequest(requests.at(i));
            _receiver->addToCachedRequests(request);
        }
    }
}

//When the server writes something
void SslConnection::onBytesWritten(qint64 bytes)
{
    if(!sender())
        return;

    qInfo().noquote() << _logPrefix << "number of bytes written" << bytes;
}

//When the socket changes state
void SslConnection::onStateChanged(QAbstractSocket::SocketState socketState)
{
    if(!sender())
        return;

    qInfo().noquote() << _logPrefix << "state" << socketState;
}

//When the socket has an error
void SslConnection::onError(QAbstractSocket::SocketError socketError)
{
    if(!sender())
        return;

    qInfo().noquote() << _logPrefix << "socketError" << socketError;
}

//When the socket has an error (ssl)
void SslConnection::onSslError(QList<QSslError> socketErrors)
{
    if(!sender())
        return;

    for(QSslError error : socketErrors)
    {
        qInfo().noquote() << _logPrefix << "socketSslError" << error.errorString();
    }
}

//Setup the current socket and connections
void SslConnection::setSocket(qintptr descriptor)
{
    _socket = new QSslSocket(this);

    connect(_socket, &QSslSocket::connected, this, &SslConnection::onConnect);
    connect(_socket, &QSslSocket::disconnected, this, &SslConnection::onDisconnect);
    connect(_socket, &QSslSocket::readyRead, this, &SslConnection::onReadyRead);
    connect(_socket, &QSslSocket::bytesWritten, this, &SslConnection::onBytesWritten);
    connect(_socket, &QSslSocket::stateChanged, this, &SslConnection::onStateChanged);
    connect(_socket, static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error), this, &SslConnection::onError);

    _socket->setSocketDescriptor(descriptor);

    //Connected is not emitted normally, it's bugged
    emit _socket->connected();

    setupSsl(_socket);

    //Set the dtpa sender of the connection
    _sender = new DTPASender(nullptr, _socket);
    _receiver = new DTPAReceiver(_sender);
}

//Setup the SSL part
void SslConnection::setupSsl(QSslSocket *socket)
{
    socket->setProtocol(QSsl::TlsV1_0);

    QByteArray key;
    QByteArray cert;

    QFile file_key(":SSL/resources/SSL/server.key");
    if(file_key.open(QIODevice::ReadOnly))
    {
        key = file_key.readAll();
        file_key.close();
    }
    else
    {
        qCritical().noquote() << file_key.errorString();
    }

    QFile file_cert(":SSL/resources/SSL/server.crt");
    if(file_cert.open(QIODevice::ReadOnly))
    {
        cert = file_cert.readAll();
        file_cert.close();
    }
    else
    {
        qCritical().noquote() << file_cert.errorString();
    }


    QSslKey ssl_key(key, QSsl::Rsa);

    QSslCertificate ssl_cert(cert);

    socket->addCaCertificate(ssl_cert);
    socket->setLocalCertificate(ssl_cert);
    socket->setPrivateKey(ssl_key);


    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslError(QList<QSslError>)));

    socket->setSocketOption(QAbstractSocket::KeepAliveOption, true );

    socket->startServerEncryption();

    if(!socket->waitForEncrypted()) {
        qCritical().noquote() << "Wait for encrypted error!";
        return;
    }
}

//Read the socket and get the packet
QString SslConnection::readSocket()
{
    /*This is for solving the bug that the client reads more than it should from the socket
      Sometimes it reads a part of the next packet also because the current packet is shorter than
      the max size (4096)*/


    _cachedBytePacket += _socket->read(DTPA::REQUEST_MAX_SIZE - _cachedBytePacket.length());
    std::string temp = _cachedBytePacket.toStdString();

    QString packet = QString::fromStdString(temp);
    int n = packet.length() > DTPA::REQUEST_MAX_SIZE ? DTPA::REQUEST_MAX_SIZE : packet.length();

    //Check if the last ascii char of the packet is a bugged one (it's only the first byte of a pair of an extended ascii char)
    while(packet[n - 1] == 65533)
    {
        packet.remove(n - 1, 1);
        n--;
    }
    //Remove the all the characters except the ones that aren't been full read
    _cachedBytePacket.remove(0, packet.toUtf8().length());

    packet = _cachedPacket + packet;

    int indexDelimiterCount = 0;
    int indexDelimiter = 0;
    int lastIndexDelimiter = 0;
    //Check how many fragments' delimiters (~) are in the packet
    while(true)
    {
        indexDelimiter = packet.indexOf(DTPA::TOKEN_FRAGMENT_DELIMITER, indexDelimiter);

        if(indexDelimiter == -1)
            break;

        indexDelimiter++;

        if(!DTPA::isByteStuffed(packet.mid(lastIndexDelimiter, indexDelimiter), indexDelimiter - lastIndexDelimiter - 1))
        {
            indexDelimiterCount++;

            lastIndexDelimiter = indexDelimiter - 1;
        }
    }
    if(indexDelimiterCount < 2)
    {
        _cachedPacket = packet;
        return "";
    }
    //If there is an uncomplete request in the packet, take only the completed ones
    else
    {
        int n = indexDelimiterCount % 2 ? -1 : 1;
        _cachedPacket = packet.mid(lastIndexDelimiter + n + 1);

        //Remove all characters from lastIndexDelimiter
        packet.remove(lastIndexDelimiter + n + 1, packet.length() - lastIndexDelimiter + 1);
        _cachedBytePacket.remove(0, lastIndexDelimiter);
    }

    return packet;
}
