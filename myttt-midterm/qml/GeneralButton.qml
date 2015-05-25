import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle
{
    id: generalButtonContainer
    width: parent.width * 0.63
    height: parent.height * 0.125
    radius: 100
    gradient: Gradient {
        GradientStop { position: 0.2; color: "light gray" }
        GradientStop { position: 0.5; color: "dark gray" }
        GradientStop { position: 0.8; color: "gray" }
    }
    border.width: 2
    border.color: "black"

    property string pText: "";

    Text
    {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: parent.width * 0.16
        text: pText
        color: "#111111"
    }
}
