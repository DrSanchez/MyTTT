#ifndef TTTCLIENT_H
#define TTTCLIENT_H

//unix includes
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

//ttt common
//#include "TTTNetworkedDef.h"

//ttt includes
#include "tttuser.h"

//qt includes
#include <QObject>
#include <QRunnable>

class TTTClient : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit TTTClient(TTTUser * user, QObject *parent = 0);

    //qml exposed property
    Q_INVOKABLE bool validateUsername(QString username);
    Q_INVOKABLE bool validateMove(int row, int col);
    Q_INVOKABLE bool validateServerIp(QString ip);

signals:
    void onUpdateClientList(QList<QString> * list);
    void invalidServerIp();
    void invalidUsername();

public slots:

protected:
    void run();

private:
    //private data members
    QString          _ip;
    TTTUser        * _localUser;
    QList<QString> * _onlineUsers;

    //private methods
    bool tryConnect();
    bool setupClient();

};

#endif // TTTCLIENT_H
