import QtQuick 2.0

Rectangle
{
    id: mainContainer
    width: ScreenWidth * 0.3
    height: ScreenHeight * 0.6
    color: "gray"

    Component.onCompleted:
    {
        startScreen.visible = true;
    }

    StartScreen
    {
        id: startScreen
        visible: false
    }
    LobbyScreen
    {
        id: lobbyScreen
        visible: false
    }
    GameScreen
    {
        id: gameScreen
        visible: false
    }

}
