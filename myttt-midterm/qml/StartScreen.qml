import QtQuick 2.0

Rectangle
{
    id: startScreenContainer
    width: parent.width
    height: parent.height
    gradient: Gradient {
        GradientStop { position: 0.01; color: "black" }
        GradientStop { position: 0.1; color: "dark gray" }
        GradientStop { position: 0.75; color: "gray" }
        GradientStop { position: 0.9; color: "black" }
    }

    Rectangle
    {
        id: titleContainer
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.05
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: parent.height * 0.2
        radius: 25
        gradient: Gradient {
            GradientStop { position: 0.1; color: "dark blue" }
            GradientStop { position: 0.8; color: "blue" }
        }
        border.width: 2
        border.color: "black"
        z: parent.z + 1

        Text
        {
            id: titleText
            anchors.centerIn: parent
            font.pixelSize: parent.width * 0.175
            text: "Tic Tac Toe"
            color: "light gray"
        }
    }

    CustomTextEdit
    {
        id: usernameEdit
        anchors.top: titleContainer.bottom
        anchors.topMargin: parent.height * 0.05
        anchors.horizontalCenter: parent.horizontalCenter
        pLabel: "Username:"
    }

    CustomTextEdit
    {
        id: ipEdit
        anchors.top: usernameEdit.bottom
        anchors.topMargin: usernameEdit.height * 0.325
        anchors.horizontalCenter: parent.horizontalCenter
        pLabel: "Server IP:"
    }

    GeneralButton
    {
        id: joinButton
        anchors.bottom: quitButton.top
        anchors.bottomMargin: parent.height * 0.033
        anchors.horizontalCenter: parent.horizontalCenter
        pText: "Join"

        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                if(Client.validateServerIp(ipEdit.pText, usernameEdit.pText))
                    mainContainer.nextAppState = "LOBBY";
            }
        }
    }

    GeneralButton
    {
        id: quitButton
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.033
        anchors.horizontalCenter: parent.horizontalCenter
        pText: "Exit"

        MouseArea
        {
            anchors.fill: parent
            onClicked: Qt.quit();
        }
    }

}
