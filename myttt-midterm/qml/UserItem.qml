import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle
{
    id: userItemContainer
    width: parent.width
    height: 75
    radius: 35
    border.width: 4
    border.color: "black"
    gradient: Gradient {
        GradientStop { position: 0.01; color: "gray" }
        GradientStop { position: 0.1; color: "lightsteelblue" }
        GradientStop { position: 0.75; color: "blue" }
        GradientStop { position: 0.9; color: "dark blue" }
    }

    Component.onCompleted: toggleCircle.setSizeOfCircle((height * 0.9), (height * 0.9), (height * 0.9) / 2);
    onHeightChanged: toggleCircle.setSizeOfCircle((height * 0.9), (height * 0.9), (height * 0.9) / 2);
    onWidthChanged: toggleCircle.setSizeOfCircle((height * 0.9), (height * 0.9), (height * 0.9) / 2);

    Text
    {
        id: userItemText
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: parent.width * 0.075
        text: model.username
        color: "white"
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle
    {
        id: toggleCircle
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.0125
        anchors.verticalCenter: parent.verticalCenter
        border.width: 4
        border.color: "black"
        gradient: Gradient {
            GradientStop { position: 0; color: !model.engagedFlag ? "dark green" : "dark red" }
            GradientStop { position: 0.5; color: !model.engagedFlag ? "light green" : "light red" }
            GradientStop { position: 1; color: !model.engagedFlag ? "dark green" : "dark red" }
        }

        function setSizeOfCircle(height, width, radius)
        {
            toggleCircle.height = height;
            toggleCircle.width = width;
            toggleCircle.radius = radius;
        }

        RectangularGlow
        {
            id: toggleButtonGlow
            height: toggleCircle.height
            width: toggleCircle.width
            color: !model.engagedFlag ? "#9911DD11" : "#99DD1111"
            glowRadius: toggleCircle.radius
        }

        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                //if user is not already engaged, we can invite them
                if (!model.engagedFlag)
                    userListViewContainer.sendChallenge(model.username);
            }
        }
    }
}
