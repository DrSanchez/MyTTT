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

        onInvalidUsername: infoDialog.showBox(2, "Invalid Username",
          "This username is already in use, please try another...");

        onServerFailedToConnect: infoDialog.showBox(3, "Cannot Connect To Server",
          "Unable to connect to server socket. Exceeded timeout attempts. Please" +
          " check the Ip address and try again");

        onAddressInfoFailure: infoDialog.showBox(3, "Address Identification Failure",
          "Could not resolve the address info for the given Ip address. Please check" +
          " that the provided address is correct and try again.");

        onServerConnected:
        {

        }
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
