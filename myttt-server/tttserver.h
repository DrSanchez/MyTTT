#ifndef TTTSERVER_H
#define TTTSERVER_H

//unix includes
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//ttt common
#include "TTTNetworkedDef.h"

//qt includes
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QRunnable>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

//ttt includes
#include "gamemanager.h"
#include "globalupdatethread.h"

#define BUFFER_MAX          1024
#define LISTEN_QUEUE        10
#define DEF_HOSTNAME_MAX    256

class TTTServer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TTTServer(QObject *parent = 0);
    ~TTTServer();

    void cleanupServer();

signals:
    void sendGlobalUpdate();

public slots:
    void issueGlobalUpdate();

protected:
    void run();

private:
    //private data members
    int                             _uniquifier;
    char                            _basicBuffer[BUFFER_MAX];
    bool                            _running;
    QByteArray                 *    _byteBuffer;
    QList<ClientObject *>      *    _clientList;
    QMap<int, GameManager *>   *    _gameMap;
    GlobalUpdateThread         *    _global;

    //unix sockets members
    int                  _fdMax;
    int                  _listener;
    int                  _serverSocket;
    fd_set               _master;
    QString              _hostname;
    struct sockaddr_in * _setup;

    //private methods
    void clearBuffer(char * buffer);
    void processMessage(ClientObject * client);
    void updateEngagedUsers(QString challenger, QString challengee, bool gameStart);

    //private command methods
    bool sendAll(int receiver);
    void sendList(int clientSock);
    void removeUser(int clientSock);
    void addUser(int clientSock, QJsonObject & obj);
    void makeMove(int clientSock, QJsonObject & obj);
    void startGame(int clientSock, QJsonObject & obj);
    void inviteUser(int clientSock, QJsonObject & obj);
    void alertForfeit(int clientSock, QJsonObject & obj);
    void declineInvite(int clientSock, QJsonObject & obj);

    int findSocketByName(QString name);
    ClientObject * findClientBySocket(int clientSock);

};

#endif // TTTSERVER_H
