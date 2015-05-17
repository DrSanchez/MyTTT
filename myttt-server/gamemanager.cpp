#include "gamemanager.h"

GameManager::GameManager(int gameID, QObject *parent)
    : QObject(parent), _gameID(gameID)
{
    //initialize the board
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            _board[i][j] = OPEN;

    //initial game state when not playing
    _state = STARTING;

}

bool GameManager::anyOpen()
{
    bool anyOpen = false;

    for (int i = 0; i < 3 && !anyOpen; i++)
        for (int j = 0; j < 3 && !anyOpen; j++)
            if (_board[i][j] == OPEN)
                anyOpen = true;

    return anyOpen;
}

bool GameManager::gameOverState()
{
    return (_state == WIN_X || _state == WIN_Y || _state == DRAW);
}

bool GameManager::checkWinDrawCondition(Player p)
{
    //check diagonal
    if((_board[1][1] != OPEN) &&
       ((_board[0][0] == _board[1][1] && _board[0][0] == _board[2][2]) ||
       (_board[0][2] == _board[1][1] && _board[0][2] == _board[2][0])))
        _state = (p == PLAYER_X ? WIN_X : WIN_Y);

    //check row/col
    for (int i = 0; i < 3 && !gameOverState(); i++)
        if((_board[i][i] != OPEN) &&
           ((_board[i][0] == _board[i][1] && _board[i][0] == _board[i][2])||
           (_board[0][i] == _board[1][i] && _board[0][i] == _board[2][i])))
            _state = (p == PLAYER_X ? WIN_X : WIN_Y);

    if (!anyOpen())
        _state = DRAW;

    return (_state == WIN_X || _state == WIN_Y || _state == DRAW);
}

bool GameManager::makeMove(Player p, int row, int col)
{
    bool valid = false;
    if (_board[row][col] == OPEN)
    {
        _board[row][col] = (p == PLAYER_X ? USED_X : USED_O);
        valid = true;
    }

    return valid;
}
