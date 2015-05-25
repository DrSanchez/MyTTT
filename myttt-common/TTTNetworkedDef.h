#ifndef TTTNETWORKEDDEF_H
#define TTTNETWORKEDDEF_H

#include <QString>

#define TTT_PORT "42040"

/* client state filter */
enum ClientState
{
    BUSY,/* responding to invite, or waiting for response */
    ENGAGED,/* user in a game */
    CONNECTED,/* idle user in lobby */
    DISCONNECTING/* user left, cleanup */
};

/* first byte of any client request to server */
enum CommHeader
{
    JOIN,
    LIST,
    LEAVE,
    INVITE,
    ACCEPTINVITE
};

/* first byte of any server response to client */
enum ServerResponse
{
    ACCEPTEDCLIENT,
    USERLIST,
    CLIENTLEFT,
    CHALLENGEACCEPTED
};

/* commands used when client is ENGAGED */
enum GameCommands
{
    MAKEMOVE

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
    int             _gameID;/* id of game, if ENGAGED: -1 if not in game */
    int             _socketID;/* socket descriptor */
    QString         _username;/* client specified username */
    ClientState     _state;

} ClientObject;

#endif // TTTNETWORKEDDEF_H
