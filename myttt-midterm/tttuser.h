#ifndef TTTUSER_H
#define TTTUSER_H

#include "TTTNetworkedDef.h"

#include <QObject>
#include <QString>

class TTTUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(ClientState state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString piece READ piece WRITE setPiece NOTIFY pieceChanged)

public:
    explicit TTTUser(QObject *parent = 0);
    ~TTTUser();

    //getters for QML property interface
    QString username();
    ClientState state();
    QString piece();

    //client is allowed to set his own name
    void setUsername(QString name);

    //client is told what piece they are
    void setPiece(QString piece);//"X" or "O"

signals:
    void gameIdChanged();
    void usernameChanged();
    void stateChanged();
    void pieceChanged();

public slots:

private:
    //private data members
    int             _socketID;/* socket descriptor */
    QString         _username;/* client specified username */
    ClientState     _state;

    //player piece
    QString         _piece;

    //only the server will ever update this values
    void setClientState(ClientState state);

};

#endif // TTTUSER_H
