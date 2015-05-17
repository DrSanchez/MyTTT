#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

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

class GameManager : public QObject
{
    Q_OBJECT
public:
    explicit GameManager(int gameID, QObject *parent = 0);

    //game logic prototypes
    bool anyOpen();
    bool gameOverState();
    bool checkWinDrawCondition(Player p);
    bool makeMove(Player p, int row, int col);

signals:
    void sendUpdateBoardState(Player p, int row, int col, int xID, int yID);
    void sendGameOverNotification(GameState g);

public slots:

private:
    //private data members
    int         _gameID;
    int         _playerXID;
    int         _playerOID;
    GameState   _state;
    TileState   _board[3][3];

};

#endif // GAMEMANAGER_H
