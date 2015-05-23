import QtQuick 2.0

Rectangle
{
    id: userItemContainer
    width: parent.width
    height: parent.height * 0.2
    anchors.horizontalCenter: parent.horizontalCenter
    radius: 100
    color: "white"
    border.width: 2
    border.color: "black"

    Text
    {
        id: userItemText
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.05
        font.pixelSize: parent.width * 0.1
        text: model.username
        color: "white"
    }
}
