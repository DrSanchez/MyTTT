#include "gamehandler.h"

GameHandler::GameHandler(QObject *parent)
    : QObject(parent)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            _boardView[i][j] = OPEN;
}

bool GameHandler::validateMove(int row, int col)
{
    if (_boardView[row][col] != OPEN)
        return false;
    else
        return true;
}
