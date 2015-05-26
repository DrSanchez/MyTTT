#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>

enum GameState
{
    DRAW,
    WIN_X,
    WIN_O,
    PLAYING
};

enum TileState
{
    OPEN,
    USED_X,
    USED_O
};

class GameManager : public QObject
{
    Q_OBJECT
public:
    explicit GameManager(int gameID, QObject *parent = 0);

    void setXID(int socket);
    void setOID(int socket);

    int getOpponentSocket(int socket);
    GameState getState();

    //game logic prototypes
    bool anyOpen();
    bool gameOverState();
    bool checkWinDrawCondition(int player);
    bool makeMove(int player, int row, int col);

signals:
    //updates the user who did not make the move
    void sendUpdateBoardState(int player, int row, int col);
    void sendGameOverNotification(GameState g);

    //error signal, probably wont happen
    void wrongGameError(int player, int gameID);

public slots:

private:
    //private data members
    int         _gameID;
    int         _playerXID;//uses socket id
    int         _playerOID;//uses socket id
    GameState   _state;
    TileState   _board[3][3];

};

#endif // GAMEMANAGER_H
