#ifndef TTTUSER_H
#define TTTUSER_H

#include "TTTNetworkedDef.h"

#include <QObject>
#include <QString>

class TTTUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int gameId READ gameId NOTIFY gameIdChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(ClientState state READ state NOTIFY stateChanged)

public:
    explicit TTTUser(QObject *parent = 0);

    //getters for QML property interface
    int id();
    int gameId();
    QString username();
    ClientState state();

    //client is allowed to set his own name
    void setUsername(QString name);

    //client is told what piece they are
    void setPiece(QString piece);//"X" or "O"

signals:
    void idChanged();
    void gameIdChanged();
    void usernameChanged();
    void stateChanged();

public slots:

private:
    //private data members
    int             _id;/* indexable id */
    int             _gameID;/* id of game, if ENGAGED: -1 if not in game */
    int             _socketID;/* socket descriptor */
    QString         _username;/* client specified username */
    ClientState     _state;

    //player piece
    QString         _piece;

    //only the server will ever update this values
    void setId(int id);
    void setGameId(int id);
    void setClientState(ClientState state);

};

#endif // TTTUSER_H
