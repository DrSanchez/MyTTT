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

    Connections
    {
        target: Client

        onNewUser:
        {
            userListModel.append({"username":name, "engagedFlag":engaged})
        }

        onRemoveUser:
        {
            var index = 0;
            var found = false;

            for (index; index < userListModel.count && found == false; index++)
            {
                var user = userListModel.get(index);
                if (user.username == name)
                {
                    found = true;
                    break;
                }
            }
            if (found == true)
                userListModel.remove(index);
        }

        onUpdateUserEngaged:
        {
            var index = 0;
            var found = false;

            for (index; index < userListModel.count && found == false; index++)
            {
                var user = userListModel.get(index);
                if (user.username == name)
                {
                    user.engagedFlag = engaged;
                    found = true;
                    break;
                }
            }
        }

        onResetUserList:
        {
            userListModel.clear();
        }
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

    Connections
    {
        target: Client

        //only challenger will receive this signal
        onChallengeAccepted://challengee knows they accepted
        {
            infoDialog.hideBox();//call this in case you didn't acknowledge, but game is starting
            //challenger is always "X"
            LocalUser.piece = "X";
            Client.localTurn = true;
            mainContainer.nextAppState = "GAME";
        }

        onChallengeDeclined:
        {
            infoDialog.showBox(1, title, message);

            //trick the message box to not switch screens in the background
            mainContainer.nextAppState = "LOBBY";//until user has acknowledged
        }

        onChallenged:
        {
            userListViewContainer.receivedChallenge(challenger)
        }
    }

    //only the challenged user will use these connections
    Connections
    {
        target: infoDialog

        //we need to notify challenger we said yes
        onClickedYes:
        {
            //opponent param tells client who we are talking to
            if (Client.acceptChallenge(gameScreen.opponent))
            {
                //receiver of challenge is always "O"
                LocalUser.piece = "O";
                Client.localTurn = false;
                mainContainer.nextAppState = "GAME";
            }
            //else error on send data
        }

        //we need to notify challenger we said no
        onClickedNo:
        {
            //opponent param tells client who we are talking to
            if (Client.declineChallenge(gameScreen.opponent))
            {
                //pretty much do nothing - ensure we are at lobby
                mainContainer.nextAppState = "LOBBY";//commonly this does nothing
            }
            //else error on send data
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

        //challengee will never call this
        function sendChallenge(userToChallenge)
        {
            if (LocalUser.username != userToChallenge)
            {//cant challenge yourself.. >.<
                if (Client.challengeUser(userToChallenge))
                {//pop wait on invite message
                    infoDialog.showBox(1, "Challenge Wait", "Waiting for response from " +
                                       userToChallenge);
                    //the challenger will be notified by a client signal from server

                    //we can safely set this, because if declined, no one sees this
                    gameScreen.opponent = userToChallenge;//and it will be set again if challenged
                    //also this is useful for having the data around
                }
            }
        }

        //challenger will never call this
        function receivedChallenge(challenger)
        {
            infoDialog.yesNoBox("Challenged!", "You have been challenged to a game" +
                                " of Tic-Tac-Toe by " + challenger);
            //box will emit signal, this is essentially a blocking ui call

            //we can safely set this, because if declined, no one sees this
            gameScreen.opponent = challenger;//and it will be set again if challenged
            //also this is useful for having the data around
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
                    Client.leaveLobby();
                    mainContainer.nextAppState = "STARTING";
                }
            }
        }
    }
}
