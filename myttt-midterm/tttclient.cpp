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
        return true;
}

bool TTTClient::validateServerIp(QString ip)
{
    _ip = ip;
    if (setupClient())
    {
        //call up user list from server
    }
    else
    {
        //error
        qDebug() << "Fall through, error in alert notification...";
        return false;
    }

    //should not reach this point
    return false;
}

bool TTTClient::validateMove(int row, int col)
{

}

bool TTTClient::setupClient()
{
    struct addrinfo * addressInfoList, * addressInfoPtr;
    struct addrinfo hint;
    int error = 0;

    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    QByteArray ipForC = _ip.toLatin1();
    char * ip = ipForC.data();
    if ((error = getaddrinfo(ip, TTT_PORT, &hint, &addressInfoList)) < 0)
    {
        qDebug() << "Error getting address info..." << TTT_PORT;
        qDebug() << "getaddrinfo error: " << gai_strerror(error);
        emit addressInfoFailure();
        return false;
    }

    for (addressInfoPtr = addressInfoList; addressInfoPtr != NULL; addressInfoPtr = addressInfoPtr->ai_next)
    {
        if (tryConnect(addressInfoPtr->ai_family, SOCK_STREAM, 0, addressInfoPtr->ai_addr, addressInfoPtr->ai_addrlen))
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
    }

    //should not reach this point
    return false;
}

bool TTTClient::tryConnect(int domain, int type, int protocol,
    const struct sockaddr * address, socklen_t addressLength)
{
    int milliseconds = 40;

    qDebug() << "Domain: " << domain;
    qDebug() << "Type: " << type;
    qDebug() << "Protocol: " << protocol;

    for (int numberTimeouts = (2500 / milliseconds); numberTimeouts > 0; numberTimeouts--)
    {
        if ((_clientDescriptor = socket(domain, type, protocol)) < 0)
        {
            qDebug() << "Got socket from server...";
            //qobject overrides global connect method from unix sockets
            if (::connect(_clientDescriptor, address, addressLength) == 0)
                return true;
            else
                return false;
        }
        QThread::msleep(milliseconds);
    }
    return false;
}
