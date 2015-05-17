#ifndef TTTNETWORKEDDEF_H
#define TTTNETWORKEDDEF_H

#include <QString>
#include <QJsonDocument>

/* client state filter */
enum ClientState
{
    BUSY,/* responding to invite, or waiting for response */
    ENGAGED,/* user in a game */
    CONNECTED,/* idle user in lobby */
    DISCONNECTING/* user left, cleanup */
};

/* first byte of any server request */
enum CommHeader
{
    JOIN,
    LIST,
    LEAVE,
    INVITE
};

/* commands used when client is ENGAGED */
enum GameCommands
{

};

/* enumeration for piece */
enum Piece
{
    X_PIECE,
    O_PIECE
};

/* describes a client connection */
typedef struct
{
    int             _id;/* indexable id */
    int             _gameID;/* id of game, if ENGAGED */
    int             _socketID;/* socket descriptor */
    QString         _username;/* client specified username */
    ClientState     _state;

} ClientObject;

#endif // TTTNETWORKEDDEF_H
