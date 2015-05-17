#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include <QObject>

enum GameState
{
    DRAW,
    WIN_X,
    WIN_Y,
    PLAYING,
    STARTING
};

enum TileState
{
    OPEN,
    USED_X,
    USED_O
};

enum Player
{
    PLAYER_X,
    PLAYER_Y
};

class GameHandler : public QObject
{
    Q_OBJECT
public:
    explicit GameHandler(QObject *parent = 0);

signals:

public slots:
    
private:
    //private data members
    int         _associatedGameID;
    int         _playerXID;
    int         _playerOID;
    GameState   _lastState;
    TileState   _boardView[3][3];

};

#endif // GAMEHANDLER_H