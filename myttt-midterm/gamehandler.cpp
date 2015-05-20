#include "gamehandler.h"

GameHandler::GameHandler(QObject *parent)
    : QObject(parent)
{
}

bool GameHandler::validateMove(int row, int col)
{
    if (_boardView[row][col] != OPEN)
        return false;
    else
        return true;
}
