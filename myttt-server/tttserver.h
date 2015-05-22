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
#include <QRunnable>
#include <QJsonObject>
#include <QJsonDocument>

//ttt includes
#include "gamemanager.h"

#define BUFFER_MAX          1024
#define LISTEN_QUEUE        10
#define DEF_HOSTNAME_MAX    256

class TTTServer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TTTServer(QObject *parent = 0);
    ~TTTServer();

signals:

public slots:

protected:
    void run();

private:
    //private data members
    char                        _basicBuffer[BUFFER_MAX];
    bool                        _running;
    QByteArray             *    _byteBuffer;
    QList<ClientObject>    *    _clientList;
    QMap<int, GameManager> *    _gameMap;

    //unix sockets members
    int                 _fdMax;
    int                 _listener;
    int                 _serverSocket;
    fd_set              _master;
    QString             _hostname;
    struct sockaddr_in * _setup;

    //private methods
    void processMessage(int dataSocket);
    void updateAllList();

    //private command methods
    void addUser(int clientSock, QJsonObject & obj);
    void sendList(int clientSock);
    void removeUser(int clientSock, QJsonObject & obj);
    void inviteUser(int clientSock, QJsonObject & obj);

};

#endif // TTTSERVER_H
