#include "tttclient.h"

#include <QDebug>

TTTClient::TTTClient(TTTUser * user, QObject *parent)
    : QObject(parent), _localUser(user), _onlineUsers(nullptr)
{
    _onlineUsers = new QList<QString>();
    _onlineUsers->append("Test");
}

bool TTTClient::validateUsername(QString username)
{
    if (_onlineUsers->contains(username))
    {
        qDebug() << "Invalid username...";

        emit invalidUsername();
        return false;
    }
    else
    {
        return true;
    }
}

bool TTTClient::validateServerIp(QString ip)
{
    _ip = ip;
    if (setupClient())
    {

    }
    else
    {
        //error
        emit invalidServerIp();
        return false;
    }
}

bool TTTClient::validateMove(int row, int col)
{

}

void TTTClient::run()
{

}

bool TTTClient::setupClient()
{
    bool result = false;



    return result;
}
