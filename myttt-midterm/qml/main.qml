import QtQuick 2.0

Rectangle
{
    id: mainContainer
    width: ScreenWidth * 0.3
    height: ScreenHeight * 0.6
    color: "gray"

    property string nextAppState: "INITIALIZING"
    property string prevAppState: "INITIALIZING"

    onNextAppStateChanged:
    {
        if (nextAppState == "INITIALIZING")
        {
            //do nothing, all logic sets init up
            //this is merely for readability
        }
        else if (nextAppState == "STARTING")
        {

        }
        else if (nextAppState == "LOBBY")
        {
            startScreen.visible = false;
            lobbyScreen.visible = true;
        }
        else if (nextAppState == "GAME")
        {

        }
    }

    Connections
    {
        target: Client

        onInvalidServerIp: infoDialog.showBox(3, "Invalid Server Ip",
          "Server connection attempt timed out. Please check that" +
         " you have correctly entered the server address and try again.")

        onInvalidUsername: infoDialog.showBox(2, "Invalid Username",
          "This username is already in use, please try another...");
    }

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
    InfoDialog
    {
        id: infoDialog
        visible: false
    }
}
