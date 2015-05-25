#include "tttclient.h"

#include <QTimer>
#include <QDebug>
#include <QThread>

TTTClient::TTTClient(TTTUser * user, QObject *parent)
    : QObject(parent), _localTurn(false), _localUser(user),
      _byteBuffer(nullptr), _gameView(nullptr), _onlineUsers(nullptr)
{
    _onlineUsers = new QList<QString>();
    _gameView = new GameHandler(this);
}

bool TTTClient::waitForRequest()
{
    int selectVal = 0;
    int readBytes = 0;
    bool actualError = false;
    bool receivedSomething = false;
    char buffer[BUFFER_MAX] = { '\0' };
    fd_set reader;

    FD_ZERO(&reader);
    FD_SET(_clientDescriptor, &reader);
    _byteBuffer->clear();

    while (!receivedSomething && !actualError)
    {
        if ((selectVal = select(_clientDescriptor + 1, &reader, NULL, NULL, NULL)) == -1)
        {
            //just kidding?
            if (errno == EINTR)
                continue;
            else
            {//actual error
                qDebug() << "Error on select call...";
                actualError = true;
            }
        }

        //no need to loop through set, we know we will only hear from one socket
        if (FD_ISSET(_clientDescriptor, &reader))
        {
            receivedSomething = true;
            readBytes = recv(_clientDescriptor, buffer, BUFFER_MAX, 0);
            qDebug() << "Bytes read: " << readBytes;
            qDebug() << "Bytes: " << buffer;
            if (readBytes < 0)
            {//error
                qDebug() << "Error on recv...";
                actualError = true;
            }
            else if (readBytes == 0)
            {//end of file, server must have closed
                cleanup();
            }
            else if (readBytes >= BUFFER_MAX)
            {//cant ever be greater, but we could handle with same logic
                char * bufferCollector;

                //deep copy the recv message
                _byteBuffer->resize(qstrlen(buffer));
                qstrcpy(_byteBuffer->data(), buffer);

                //won't ever actually be greater than max, but QByteArray could handle the case
                while (readBytes >= BUFFER_MAX)
                {
                    readBytes = recv(_clientDescriptor, buffer, BUFFER_MAX, 0);

                    //ensure any fragmented messages are all deep copied into our buffers
                    bufferCollector = new char[_byteBuffer->size() + qstrlen(buffer) + 1];
                    qstrcpy(bufferCollector, _byteBuffer->data());
                    strcat(bufferCollector, buffer);
                    _byteBuffer->resize(qstrlen(bufferCollector));
                    qstrcpy(_byteBuffer->data(), bufferCollector);
                }
                processServerResponse();
            }
            else //if (readBytes < BUFFER_MAX)
            {//we probably got a full message
                _byteBuffer->resize(qstrlen(buffer) + 1);
                qstrcpy(_byteBuffer->data(), buffer);
                processServerResponse();
            }
        }
    }
    if (actualError)
        return actualError;

    return receivedSomething;
}

void TTTClient::cleanup()
{

}

bool TTTClient::localTurn()
{
    return _localTurn;
}

void TTTClient::setLocalTurn(bool turn)
{
    if (_localTurn != turn)
    {
        _localTurn = turn;
        emit localTurnChanged();
    }
}

bool TTTClient::validateServerIp(QString ip, QString username)
{
    bool result = false;
    _ip = ip;
    _localUser->setUsername(username);
    if (setupClient())
    {
        //call up user list from server
        qDebug() << "Client should be connected...";
        if (sendUser())
        {
            if(!waitForRequest())
            {//this likely will never occur, due to the uniquifier
                qDebug() << "Username validation broken?";
                emit invalidUsername();
                return false;
            }
            if (requestUserList())
            {
                //gotta wait to hear back...
                if (!waitForRequest())
                    return false;
                else
                    result = true;
            }
        }
    }
    else
    {
        //error
        qDebug() << "Fall through, error in alert notification...";
        result = false;
    }
    return result;
}

bool TTTClient::validateMove(int row, int col)
{
    return _gameView->validateMove(row, col, _localUser->piece());
}

bool TTTClient::challengeUser(QString challengedUser)
{
    bool result = false;
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bytes;

    obj["CommHeader"] = INVITE;
    obj["Inviter"] = _localUser->username();
    obj["Invitee"] = challengedUser;

    doc.setObject(obj);
    bytes = doc.toJson();

    if (!sendAll(bytes))
    {
        //error
        qDebug() << "Error sending all bytes...";
        result = false;
    }
    else
        result = true;

    return result;
}

bool TTTClient::setupClient()
{
    struct sockaddr_in * serverSetup;

    serverSetup = (struct sockaddr_in *)malloc(sizeof(serverSetup));
    serverSetup->sin_family = AF_INET;
    serverSetup->sin_addr.s_addr = htonl(inet_network(_ip.toStdString().data()));
    serverSetup->sin_port = htons(42040);


    if (tryConnect(serverSetup->sin_family, SOCK_STREAM, 0, (struct sockaddr *)serverSetup))
        return true;
    else
    {
        emit serverFailedToConnect();
        return false;
    }

    //should not reach this point
    return false;
}

bool TTTClient::tryConnect(int domain, int type, int protocol, struct sockaddr * address)
{
    int milliseconds = 40;
    bool connected = false;

    qDebug() << "Domain: " << domain;
    qDebug() << "Type: " << type;
    qDebug() << "Protocol: " << protocol;

    for (int numberTimeouts = (2500 / milliseconds); numberTimeouts > 0 && !connected; numberTimeouts--)
    {
        if ((_clientDescriptor = socket(domain, type, protocol)) >= 0)
        {
            //qobject overrides global connect method from unix sockets
            if (::connect(_clientDescriptor, address, sizeof(*address)) >= 0)
                connected = true;
        }
        QThread::msleep(milliseconds);
    }
    return connected;
}

bool TTTClient::requestUserList()
{
    bool result = false;
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bytes;
    
    obj["CommHeader"] = LIST;
    
    doc.setObject(obj);
    bytes = doc.toJson();
    
    if (!sendAll(bytes))
    {
        //error
        qDebug() << "Error sending all bytes...";
        result = false;
    }
    else
        result = true;

    return result;
}

bool TTTClient::sendUser()
{
    bool result = false;
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bytes;

    obj["CommHeader"] = JOIN;
    obj["Username"] = _localUser->username();

    doc.setObject(obj);
    bytes = doc.toJson();

    if (!sendAll(bytes))
    {
        //error
        qDebug() << "Error sending all bytes...";
        result = false;
    }
    else
        result = true;

    return result;
}

bool TTTClient::sendAll(QByteArray & bytes)
{
    bool success = true;
    int bytesWritten = 0;
    int bytesReturned = 0;
    int messageFullLength = bytes.length();

    qDebug() << "Send all: " << bytes << "\nFrom: " << _clientDescriptor;
    do
    {
        bytesReturned = send(_clientDescriptor, bytes.data(), bytes.length(), 0);
        if (bytesReturned < 0)
        {
            success = false;
            break;
        }
        bytesWritten += bytesReturned;
        if (bytesWritten < messageFullLength)
            bytes.remove(0, bytesReturned - 1);
        bytesReturned = 0;
    } while (bytesWritten < messageFullLength && success);
    return success;
}

void TTTClient::processServerResponse()
{
    QJsonDocument d = QJsonDocument::fromJson(*_byteBuffer);
    QJsonObject o = d.object();

    ServerResponse response = (ServerResponse) o["ServerResponse"].toInt();
    qDebug() << "Processing server response..." << *_byteBuffer;
    switch(response)
    {
        case ACCEPTEDCLIENT:
        {
            handleAcceptedClient(o);
            break;
        }
        case USERLIST:
        {
            handleUserList(o);
            break;
        }
        case CLIENTLEFT:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

void TTTClient::handleUserList(QJsonObject & obj)
{
    //getting usernames
    QJsonValue val = obj["Userlist"];
    QVariant var = val.toVariant();
    QStringList userList = var.toStringList();

    //getting each user status
    val = obj["Statuslist"];
    var = val.toVariant();
    QStringList statusList = var.toStringList();

    if (userList.length() != statusList.length())
    {//this shouldn't happen
        qDebug() << "Userlist did not get built correctly...";
    }
    else
    {
        for (int i = 0; i < userList.length(); i++)
        {
            _onlineUsers->append(userList.at(i));
            emit newUser(userList.at(i), (statusList.at(i) == "true" ? true : false));
        }
    }
}

void TTTClient::handleClientLeft(QJsonObject & obj)
{

}

void TTTClient::handleAcceptedClient(QJsonObject & obj)
{
    qDebug() << "Do we get to handle accept client?";
    QString clientName = obj["Username"].toString();

    //we are the accepted client, but our name was not unique, update it with server change
    if (clientName.contains(_localUser->username()) && clientName != _localUser->username())
        _localUser->setUsername(clientName);
    else if (clientName != _localUser->username())//we want to add this client to our lists
    {
        _onlineUsers->append(clientName);
        emit newUser(clientName, false);
    }
    //else the local user doesnt do anything except change menus
}
