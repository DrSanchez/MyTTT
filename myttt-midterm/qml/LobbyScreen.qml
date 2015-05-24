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
        id: headerContainer
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: parent.height * 0.155
        radius: 25
        z: parent.z + 1
        gradient: Gradient {
            GradientStop { position: 0.1; color: "dark blue" }
            GradientStop { position: 0.8; color: "blue" }
        }
        border.width: 3
        border.color: "black"
        Text
        {
            id: titleText
            anchors.centerIn: parent
            font.pixelSize: parent.width * 0.125
            text: "User Lobby"
            color: "light gray"
        }
    }

    Rectangle
    {
        id: userListViewContainer
        width: parent.width
        height: (parent.height - headerContainer.height)
        anchors.top: headerContainer.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        z: headerContainer.z - 1
        color: "transparent"
        border.width: 3
        border.color: "black"
        radius: 25
        clip: true

        function sendChallenge(userToChallenge)
        {
            if (Client.challengeUser(userToChallenge))
            {//pop wait on invite message
                //if receive response
                    //true == open game screen
                //else
                    //false == close message and return to lobby
                mainContainer.nextAppState = "GAME";
            }
        }

        ListView
        {
            id: userListView
            anchors.fill: parent
            model: userListModel
            orientation: Qt.Vertical
            verticalLayoutDirection: ListView.TopToBottom
            clip: true
            delegate: UserItem{}
        }

        ListModel
        {
            id: userListModel


            //temporary to test delegate
            ListElement
            {
                username: "User1"
                engagedFlag: false //or true
            }
            ListElement
            {
                username: "UsernameLongOne"
                engagedFlag: true //or false
            }
        }
    }

    Rectangle
    {
        id: backButtonUnderlay
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
            id: backButton
            height: parent.height * 0.4
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: parent.height * -0.22
            anchors.horizontalCenter: parent.horizontalCenter
            pText: "Back"
            z: headerContainer.z

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                   // Client.resetUser
                    mainContainer.nextAppState = "STARTING";
                }
            }
        }
    }
}
