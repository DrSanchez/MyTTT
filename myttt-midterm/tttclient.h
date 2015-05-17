#ifndef TTTCLIENT_H
#define TTTCLIENT_H

//unix includes
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

//ttt common
#include "TTTNetworkedDef.h"

//qt includes
#include <QObject>
#include <QRunnable>

class TTTClient : public QObject, QRunnable
{
    Q_OBJECT
public:
    explicit TTTClient(QObject *parent = 0);

signals:

public slots:

protected:
    void run();

private:
    //private data members

};

#endif // TTTCLIENT_H
