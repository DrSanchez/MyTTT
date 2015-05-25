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
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

#define BUFFER_MAX          1024

class TTTClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool localTurn READ localTurn NOTIFY localTurnChanged)

public:
    explicit TTTClient(TTTUser * user, QObject *parent = 0);

    //qml exposed methods
    Q_INVOKABLE bool validateMove(int row, int col);
    Q_INVOKABLE bool validateServerIp(QString ip, QString username);
    Q_INVOKABLE bool challengeUser(QString challengedUser);

    //property getters
    bool localTurn();

signals:
    //qml interface signals
    void invalidUsername();
    void serverConnected();
    void addressInfoFailure();
    void serverFailedToConnect();

    //property signals
    void localTurnChanged();

    //server signals
    void newUser(QString name, bool engaged);


public slots:
    //client responses


private:
    //private data members
    int              _clientDescriptor;
    bool             _localTurn;
    QString          _ip;
    TTTUser        * _localUser;
    QByteArray     * _byteBuffer;
    GameHandler    * _gameView;
    QList<QString> * _onlineUsers;

    //private methods
    void cleanup();
    bool sendAll(QByteArray &bytes);
    bool sendUser();
    bool setupClient();
    bool waitForRequest();
    bool requestUserList();
    void setLocalTurn(bool turn);
    void processServerResponse();
    bool tryConnect(int domain, int type, int protocol, sockaddr *address);

    //server response methods
    void handleUserList(QJsonObject & obj);
    void handleClientLeft(QJsonObject & obj);
    void handleAcceptedClient(QJsonObject & obj);

};

#endif // TTTCLIENT_H
