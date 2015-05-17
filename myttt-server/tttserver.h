#ifndef TTTSERVER_H
#define TTTSERVER_H

//unix includes
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

//ttt common
#include "TTTNetworkedDef.h"

//qt includes
#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QJsonDocument>

//ttt includes
#include "gamemanager.h"

class TTTServer : public QObject, QRunnable
{
    Q_OBJECT
public:
    explicit TTTServer(QObject *parent = 0);

signals:

public slots:

protected:
    void run();

private:
    //private data members
    QList<ClientObject> * _clientList;
    QMap<int, GameManager> * _gameList;

    //unix sockets members
    struct sockaddr *   _address;
    socklen_t           _addressLength;
    struct addrinfo *   _addressInfoPtr;
    struct addrinfo *   _addressInfoList;

};

#endif // TTTSERVER_H
