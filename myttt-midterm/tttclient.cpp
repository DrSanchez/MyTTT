#include "tttclient.h"

#include <QTimer>
#include <QDebug>
#include <QThread>

TTTClient::TTTClient(TTTUser * user, QObject *parent)
    : QObject(parent), _localUser(user), _onlineUsers(nullptr)
{
    _onlineUsers = new QList<QString>();
}

bool TTTClient::validateUsername(QString username)
{
    if (_onlineUsers->contains(username))
    {
        //error
        emit invalidUsername();
        return false;
    }
    else
    {
        _localUser->setUsername(username);
        return true;
    }
}

bool TTTClient::validateServerIp(QString ip)
{
    bool result = false;
    _ip = ip;
    if (setupClient())
    {
        //call up user list from server
        qDebug() << "Client should be connected...";
        result = true;
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
    return _gameView->validateMove(row, col);
}

bool TTTClient::challengeUser(QString challengedUser)
{
    bool result = false;
//    QJsonObject obj;
//    QJsonDocument doc;
//    QByteArray bytes;

//    obj["CommHeader"] = INVITE;
//    obj["Inviter"] = _localUser->username();
//    obj["Invitee"] = challengedUser;

//    doc.setObject(obj);
//    bytes = doc.toBinaryData();

//    if (!sendAll(bytes))
//    {
//        //error
//        qDebug() << "Error sending all bytes...";
//        result = false;
//    }
//    else
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
    {
        //we have connected
        //notify connect
        return true;
    }
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
    bytes = doc.toBinaryData();
    
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
    bytes = doc.toBinaryData();

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

bool TTTClient::sendAll(QByteArray bytes)
{
    bool success = true;
    int bytesWritten = 0;
    int bytesReturned = 0;
    int messageFullLength = bytes.length();
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
            bytes.remove(0, bytesWritten - 1);
    } while (bytesWritten < messageFullLength && success);
    
    return success;
}



