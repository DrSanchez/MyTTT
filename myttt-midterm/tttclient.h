#ifndef TTTCLIENT_H
#define TTTCLIENT_H

//unix includes
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//ttt common
//#include "TTTNetworkedDef.h"

//ttt includes
#include "tttuser.h"
#include "gamehandler.h"

//qt includes
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

class TTTClient : public QObject
{
    Q_OBJECT

public:
    explicit TTTClient(TTTUser * user, QObject *parent = 0);

    //qml exposed property
    Q_INVOKABLE bool validateUsername(QString username);
    Q_INVOKABLE bool validateMove(int row, int col);
    Q_INVOKABLE bool validateServerIp(QString ip);

signals:
    //qml interface signals
    void invalidUsername();
    void serverConnected();
    void addressInfoFailure();
    void serverFailedToConnect();

    //server signals


public slots:
    //client responses


private:
    //private data members
    int              _clientDescriptor;
    QString          _ip;
    TTTUser        * _localUser;
    GameHandler    * _gameView;
    QList<QString> * _onlineUsers;

    //private methods
    bool sendAll(QByteArray bytes);
    bool sendUser();
    bool setupClient();
    bool requestUserList();
    bool tryConnect(int domain, int type, int protocol, sockaddr *address);

};

#endif // TTTCLIENT_H
