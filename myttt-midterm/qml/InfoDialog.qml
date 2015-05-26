import QtQuick 2.0

Rectangle
{
    id: transparencyContainer
    width: parent.width
    height: parent.height
    anchors.centerIn: parent
    color: "transparent"
    z: parent.z + 1
    visible: false

    property int alertLevel: 0
    property string alertMessage: ""
    property string alertTitle: ""

    signal clickedYes
    signal clickedNo

    function showBox(level, title, message)
    {
        //if ()
        transparencyContainer.alertLevel = level;
        if (level == 1)
            alertLevelIconContainer.source = "../images/information_icon.png";
        else if (level == 2)
            alertLevelIconContainer.source = "../images/warning_icon.png";
        else if (level == 3)
            alertLevelIconContainer.source = "../images/error_icon.png";

        transparencyContainer.alertTitle = title;
        transparencyContainer.alertMessage = message;
        transparencyContainer.visible = true;
        applicationShadingAnimationIn.start();
    }

    function yesNoBox(title, message)
    {
        //yes/no question will be considered informational to user
        transparencyContainer.alertLevel = 1;
        alertLevelIconContainer.source = "../images/information_icon.png";
        transparencyContainer.alertTitle = title;
        transparencyContainer.alertMessage = message;
        yesButton.visible = true;//need to show/activate yes button
        applicationShadingAnimationIn.start();
    }

    function hideBox()
    {
        transparencyContainer.alertLevel = 0;
        transparencyContainer.alertTitle = "";
        transparencyContainer.alertMessage = "";
        transparencyContainer.visible = false;
        applicationShadingAnimationOut.start();
        mainContainer.nextAppState = mainContainer.prevAppState;
    }

    Rectangle
    {
        id: applicationShading
        height: parent.height
        width: parent.width
        anchors.centerIn: parent
        color: "black"
        opacity: 0.5
        z: parent.z + 1

        PropertyAnimation
        {
            id: applicationShadingAnimationIn
            property: "opacity"
            target: applicationShading
            from: 0
            to: 0.5
            duration: 500
            alwaysRunToEnd: true
        }
        PropertyAnimation
        {
            id: applicationShadingAnimationOut
            property: "opacity"
            target: applicationShading
            from: 0.5
            to: 0
            duration: 500
            alwaysRunToEnd: true
        }
    }

    Rectangle
    {
        id: alertBoxContainer
        width: parent.width * 0.9
        height: parent.height * 0.3
        anchors.centerIn: parent
        gradient: Gradient
        {
            GradientStop { position: 0.15; color: "light gray" }
            GradientStop { position: 0.4; color: "gray" }
            GradientStop { position: 0.85; color: "gray" }
            GradientStop { position: 0.95; color: "black" }
        }
        border.width: 3
        border.color: "black"
        radius: 10
        z: parent.z + 100//message box should always be on top

        Rectangle
        {
            id: alertBoxTitleBar
            height: parent.height * 0.125
            width: parent.width
            radius: 10
            color: "gray"
            border.width: 3
            border.color: "black"
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            Text
            {
                id: alertTitleText
                font.pixelSize: parent.height * 0.7
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: parent.width * 0.05
                text: transparencyContainer.alertTitle
                antialiasing: true
            }
        }

        Image
        {
            id: alertLevelIconContainer
            height: parent.height * 0.3
            fillMode: Image.PreserveAspectFit
            source: "../images/information_icon.png"
            anchors.top: alertMessageContainer.top
            anchors.right: alertMessageContainer.left
        }

        Rectangle
        {
            id: alertMessageContainer
            height: parent.height * 0.5
            width: parent.width * 0.6
            anchors.top: alertBoxTitleBar.bottom
            anchors.topMargin: parent.height * 0.075
            anchors.horizontalCenter: parent.horizontalCenter
            color: "transparent"

            Text
            {
                id: alertMessageText
                font.pixelSize: alertTitleText.font.pixelSize
                wrapMode: Text.Wrap
                anchors.fill: parent
                anchors.margins: parent.height * 0.05
                text: transparencyContainer.alertMessage
           }
        }

        GeneralButton
        {
            id: yesButton
            height: parent.height * 0.18
            width: parent.width * 0.22
            anchors.right: okButton.left
            anchors.rightMargin: parent.width * 0.12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height * 0.12
            pText: "Yes"
            visible: false //default behavior does not involve yes/no response

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    if (yesButton.visible == true)
                    {
                        transparencyContainer.clickedYes();
                        transparencyContainer.hideBox();
                        yesButton.visible = false;
                    }
                }
            }
        }

        GeneralButton
        {
            id: okButton //sometimes this is "noButton"
            height: parent.height * 0.18
            width: parent.width * 0.22
            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height * 0.12
            pText: "OK"

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    if (okButton.pText == "No")
                    {
                        transparencyContainer.clickedNo();
                        okButton.pText = "OK"; //return to default info behavior
                    }
                    transparencyContainer.hideBox();
                }
            }
        }
    }
}
