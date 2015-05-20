import QtQuick 2.0

Rectangle
{
    id: generalButtonContainer
    width: parent.width * 0.63
    height: parent.height * 0.125
    radius: 100
    color: "gray"
    border.width: 2
    border.color: "black"

    property string pText: "";

    Text
    {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: parent.width * 0.16
        text: pText
        color: "light gray"
    }
}
