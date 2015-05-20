import QtQuick 2.0

Rectangle
{
    id: customTextEditContainer
    width: parent.width * 0.7
    height: parent.height * 0.15
    radius: 100
    clip: false
    color: "light gray"
    border.width: 2
    border.color: "black"

    property string pLabel: "";
    property string pText: customTextEdit.text

    Text
    {
        id: customTextEditLabel
        text: pLabel
        font.pixelSize: parent.width * 0.055
        anchors.bottom: parent.top
        anchors.bottomMargin: parent.height * 0.05
        anchors.horizontalCenter: parent.horizontalCenter
        color: "yellow"
    }

    //so we can clip without losing label
    Rectangle
    {
        id: textContainer
        width: parent.width
        height: parent.height
        color: "transparent"
        anchors.centerIn: parent
        clip: true

        TextEdit
        {
            id: customTextEdit
            width: parent.width
            height: parent.height
            font.pixelSize: parent.width * 0.105
            anchors.centerIn: parent.Center
            color: "black"
            textMargin: parent.width * 0.05
            verticalAlignment: TextEdit.AlignHCenter
            horizontalAlignment: TextEdit.AlignVCenter
            wrapMode: TextEdit.NoWrap
        }

    }
}
