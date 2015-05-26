#include "tttclient.h"

#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QThreadPool>

TTTClient::TTTClient(TTTUser * user, QObject *parent)
    : QObject(parent), _localTurn(false), _running(false), _localUser(user),
      _byteBuffer(nullptr), _gameView(nullptr), _onlineUsers(nullptr)
{
    _byteBuffer = new QByteArray();
    _onlineUsers = new QList<QString>();
    _gameView = new GameHandler(this);
}

//this method performs the server handshaking on startup
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
            if (readBytes < 0)
            {//error
                perror("Recv error...");
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

//this method keeps the reader thread alive
void TTTClient::run()
{
    int selectVal = 0;
    int readBytes = 0;
    char buffer[BUFFER_MAX] = { '\0' };
    fd_set reader;

    FD_ZERO(&reader);
    FD_SET(_clientDescriptor, &reader);
    _byteBuffer->clear();

    while (_running)
    {
        FD_ZERO(&reader);
        FD_SET(_clientDescriptor, &reader);
        _byteBuffer->clear();
        clearBuffer(buffer);
        if ((selectVal = select(_clientDescriptor + 1, &reader, NULL, NULL, NULL)) == -1)
        {
            //just kidding?
            if (errno == EINTR)
                continue;
            else
            {//actual error
                qDebug() << "Error on select call...";
            }
        }

        //no need to loop through set, we know we will only hear from one socket
        if (FD_ISSET(_clientDescriptor, &reader))
        {
            readBytes = recv(_clientDescriptor, buffer, BUFFER_MAX, 0);
            if (readBytes < 0)
            {//error
                perror("Recv error...");
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
}

void TTTClient::cleanup()
{
    _running = false;
    QThreadPool::globalInstance()->waitForDone(3000);
    //client reader thread should close quickly

    shutdown(_clientDescriptor, 2);//shut it all down
    close(_clientDescriptor);

    _byteBuffer->clear();
    delete _byteBuffer;
    _byteBuffer = nullptr;
    _onlineUsers->clear();
    delete _onlineUsers;
    _onlineUsers = nullptr;
    delete _gameView;
    _gameView = nullptr;
    delete _localUser;
    _localUser = nullptr;
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

bool TTTClient::acceptChallenge(QString name)
{
    bool result = false;

    QJsonObject o;
    QJsonDocument d;
    QByteArray b;

    o["CommHeader"] = ACCEPTINVITE;
    o["Receiver"] = name;

    d.setObject(o);
    b = d.toJson();

    if (sendAll(b))
    {
        result = true;
    }
    else
        qDebug() << "Error sending decline invite to server...";

    return result;
}

bool TTTClient::declineChallenge(QString name)
{
    bool result = false;

    QJsonObject o;
    QJsonDocument d;
    QByteArray b;

    o["CommHeader"] = DECLINEINVITE;
    o["Receiver"] = name;

    d.setObject(o);
    b = d.toJson();

    if (sendAll(b))
    {
        result = true;
    }
    else
        qDebug() << "Error sending decline invite to server...";

    return result;
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
                emit invalidUsername();
                return false;
            }
            //a new user will never be engaged
            emit newUser(_localUser->username(), false);
            if (requestUserList())
            {
                //gotta wait to hear back...
                if (!waitForRequest())
                    return false;
                else
                    result = true;
                //we are ready to begin monitoring for communications
                _running = true;
                QThreadPool::globalInstance()->start(this);
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
    bool result = false;

    result = _gameView->validateMove(row, col, _localUser->piece());

    if (result)
        setLocalTurn(false);

    return result;
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

    //prepare gamehandler with some info, can be overridden in invite declined
    _gameView->setPlayerX(_localUser->username());
    _gameView->setPlayerO(challengedUser);

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

void TTTClient::leaveLobby()
{
    QJsonObject o;
    QJsonDocument d;
    QByteArray bytes;

    o["CommHeader"] = LEAVE;

    d.setObject(o);
    bytes = d.toJson();

    if (!sendAll(bytes))
        qDebug() << "Error sending bytes...";
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
    bool done = false;
    int bytesWritten = 0;
    int bytesReturned = 0;
    int messageFullLength = bytes.length();

    qDebug() << "Send all: " << bytes << "\nFrom: " << _clientDescriptor;
    do
    {
        bytesReturned = send(_clientDescriptor, bytes.data(), bytes.length(), 0);
        if (bytesReturned < 0)
        {
            done = true;
            break;
        }
        bytesWritten += bytesReturned;
        if (bytesWritten < messageFullLength)
            bytes.remove(0, bytesReturned - 1);
        bytesReturned = 0;
        if (bytesWritten == messageFullLength)
            done = true;
    } while (bytesWritten < messageFullLength && !done);

    return done;
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
            handleClientLeft(o);
            break;
        }
        case CHALLENGED:
        {
            handleChallenge(o);
            break;
        }
        case CHALLENGEACCEPTED:
        {
            handleChallengeAccepted(o);
            break;
        }
        case CHALLENGEDECLINED:
        {
            handleChallengeDeclined(o);
            break;
        }
        case UPDATEENGAGED:
        {
            handleUpdateEngaged(o);
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
        //reset UI and stored list, updating from full server list
        emit resetUserList();
        _onlineUsers->clear();
        for (int i = 0; i < userList.length(); i++)
        {
            _onlineUsers->append(userList.at(i));
            emit newUser(userList.at(i), (statusList.at(i) == "true" ? true : false));
        }
    }
}

void TTTClient::handleClientLeft(QJsonObject & obj)
{
    QString leftClient = obj["Username"].toString();

    if (leftClient == _localUser->username())
    {//the server has told us it is ok to cleanup
        cleanup();
    }
    else //we were told someone left
    {
        //remove from memory
        _onlineUsers->removeOne(leftClient);

        //remove from qml view
        emit removeUser(leftClient);
    }
}

void TTTClient::handleReceiveMove(QJsonObject & obj)
{

}

void TTTClient::handleUpdateEngaged(QJsonObject & obj)
{
    //getting usernames
    QJsonValue val = obj["Users"];
    QVariant var = val.toVariant();
    QStringList userList = var.toStringList();

    //getting each user status
    val = obj["Status"];
    var = val.toVariant();
    QStringList statusList = var.toStringList();

    if (userList.length() != statusList.length())
    {//this shouldn't happen
        qDebug() << "Userlist did not get built correctly...";
    }
    else
    {
        for (int i = 0; i < userList.length(); i++)
            emit updateUserEngaged(userList.at(i), (statusList.at(i) == "true" ? true : false));
    }
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
        qDebug() << "Adding user: " << clientName;
        _onlineUsers->append(clientName);
        emit newUser(clientName, false);
    }
    //else the local user doesnt do anything except change menus
}

void TTTClient::handleChallenge(QJsonObject & obj)
{
    QString challenger = obj["Challenger"].toString();
    emit challenged(challenger);
}

void TTTClient::handleChallengeAccepted(QJsonObject & obj)
{
    emit challengeAccepted();
}

void TTTClient::handleChallengeDeclined(QJsonObject & obj)
{
    ChallengeDeclineReason reason = (ChallengeDeclineReason) obj["Reason"].toInt();

    emit challengeDeclined();
}

void TTTClient::clearBuffer(char * buffer)
{
    for (int i = 0; i < BUFFER_MAX; i++)
        buffer[i] = '\0';
}
