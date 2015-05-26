import QtQuick 2.0

Rectangle
{
    id: gameScreenContainer
    width: parent.width
    height: parent.height
    gradient: Gradient {
        GradientStop { position: 0.01; color: "black" }
        GradientStop { position: 0.1; color: "dark gray" }
        GradientStop { position: 0.75; color: "gray" }
        GradientStop { position: 0.9; color: "black" }
    }

    property string opponent: ""

    Board
    {
        id: board
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.025
        anchors.horizontalCenter: parent.horizontalCenter

        Grid
        {
            id: boardView
            width: parent.width
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: parent.width * -0.001
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: parent.width * -0.01
            columns: 3
            columnSpacing: parent.height * 0.0225
            rowSpacing: parent.height * 0.0185
            z: parent.z + 1

            Repeater
            {
                model: 9

                Tile
                {
                    width: boardView.width * 0.33
                    height: boardView.height * 0.33

                    onActivated:
                    {
                        if (Client.localTurn)
                        {
                            var row = (index % 3);
                            var col = (index - row) / 3;
                            if (Client.validateMove(row, col))
                            {
                                state = LocalUser.piece;
                            }
                        }
                    }
                }
            }
        }
    }//end of board element

    Text
    {
        id: opponentName
        font.pixelSize: parent.width * 0.05
        color: "yellow"
        text: "Opponent: " + opponent
        anchors.bottom: forfeitButtonUnderlay.top
        anchors.bottomMargin: parent.height * 0.002
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.005
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Text
    {
        id: playerTurnText
        font.pixelSize: parent.width * 0.05
        color: "yellow"
        text: "It's Your Turn!"
        anchors.bottom: forfeitButtonUnderlay.top
        anchors.bottomMargin: parent.height * 0.002
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.005
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: Client.localTurn
    }

    Rectangle
    {
        id: forfeitButtonUnderlay
        width: parent.width * 1.025
        height: parent.height * 0.3
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * -0.125
        anchors.horizontalCenter: parent.horizontalCenter
        gradient: Gradient {
            GradientStop { position: 0.1; color: "dark red" }
            GradientStop { position: 0.3; color: "red" }
            GradientStop { position: 0.55; color: "dark red" }
            GradientStop { position: 0.9; color: "black" }
        }
        border.width: 3
        border.color: "black"
        radius: 100

        GeneralButton
        {
            id: forfeitButton
            height: parent.height * 0.4
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: parent.height * -0.22
            anchors.horizontalCenter: parent.horizontalCenter
            pText: "Forfeit"
            z: parent.z + 1

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    //notify server of forfeit
                    mainContainer.nextAppState = "LOBBY";
                }
            }
        }
    }

}
