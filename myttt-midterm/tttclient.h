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
#include <QRunnable>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

#define BUFFER_MAX          1024

class TTTClient : public QObject, public QRunnable
{
    Q_OBJECT

    Q_PROPERTY(bool localTurn READ localTurn WRITE setLocalTurn NOTIFY localTurnChanged)

public:
    explicit TTTClient(TTTUser * user, QObject *parent = 0);

    //qml exposed methods
    Q_INVOKABLE void forfeit();
    Q_INVOKABLE void leaveLobby();
    Q_INVOKABLE bool acceptChallenge(QString name);
    Q_INVOKABLE bool declineChallenge(QString name);
    Q_INVOKABLE bool validateMove(int row, int col);
    Q_INVOKABLE bool challengeUser(QString challengedUser);
    Q_INVOKABLE bool validateServerIp(QString ip, QString username);

    //property setters
    void setLocalTurn(bool turn);

    //property getters
    bool localTurn();

signals:
    //qml interface signals
    void clearUIBoard();//let ui know we cleaned up the model
    void resetUserList();
    void invalidUsername();
    void addressInfoFailure();
    void serverFailedToConnect();

    //Challenge Accepted! Self-Five!
    void challengeAccepted();
    void challengeDeclined(QString title, QString message);
    void challenged(QString challenger);

    //property signals
    void localTurnChanged();

    //server signals
    void removeUser(QString name);
    void newUser(QString name, bool engaged);
    void updateUserEngaged(QString name, bool engaged);
    void updateUIBoard(int row, int col, QString piece);
    void gameoverNotification(QString title, QString message);

public slots:
    //client responses

protected:
    void run();

private:
    //private data members
    int              _clientDescriptor;
    bool             _localTurn;
    bool             _running;
    QString          _ip;
    TTTUser        * _localUser;
    QByteArray     * _byteBuffer;
    GameHandler    * _gameView;
    QList<QString> * _onlineUsers;

    //private methods
    void cleanup();
    bool sendUser();
    bool setupClient();
    bool waitForRequest();
    bool requestUserList();
    void processServerResponse();
    bool sendAll(QByteArray &bytes);
    void clearBuffer(char * buffer);
    bool tryConnect(int domain, int type, int protocol, sockaddr *address);

    //server response methods
    void handleForfeit(QJsonObject & obj);
    void handleGameover(QJsonObject & obj);
    void handleUserList(QJsonObject & obj);
    void handleChallenge(QJsonObject & obj);
    void handleClientLeft(QJsonObject & obj);
    void handleReceiveMove(QJsonObject & obj);
    void handleUpdateEngaged(QJsonObject & obj);
    void handleAcceptedClient(QJsonObject & obj);
    void handleChallengeAccepted(QJsonObject & obj);
    void handleChallengeDeclined(QJsonObject & obj);

};

#endif // TTTCLIENT_H
