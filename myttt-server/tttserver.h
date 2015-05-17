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

//ttt includes
#include "gamemanager.h"

#define BUFFER_MAX          1024
#define LISTEN_QUEUE        10
#define DEF_HOSTNAME_MAX    256

class TTTServer : public QObject, QRunnable
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
    struct sockaddr *   _address;
    socklen_t           _addressLength;
    struct addrinfo *   _addressInfoPtr;
    struct addrinfo *   _addressInfoList;

    //private methods
    void setupServer();
    void processMessage();

};

#endif // TTTSERVER_H
