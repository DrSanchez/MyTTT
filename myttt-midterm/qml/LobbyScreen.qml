import QtQuick 2.0

Rectangle
{
    id: lobbyScreenContainer
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
        id: lobbyHeader
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.0125
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: parent.height * 0.125
        radius: 25
        opacity: 0.9
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
            text: "User Lobby"
            color: "light gray"
        }
    }

    Rectangle
    {
        id: userListViewContainer
        width: parent.width
        height: (parent.height * 0.8) - lobbyHeader.height
        anchors.top: lobbyHeader.bottom

        ListView
        {
            id: userListView
        }
    }

}
