import QtQuick 2.0

Rectangle 
{
    id: boardContainer
    width: parent.width * 0.925
    height: parent.height * 0.735
    border.width: 2
    border.color: "black"
    radius: 35
    color: "transparent"

    Row
    {
        width: parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.305
        spacing: parent.height * 0.315

        Repeater
        {
            model: 2

            Rectangle
            {
                id: colBar
                width: parent.width * 0.05
                height: parent.height
                radius: 75
                gradient: Gradient {
                    GradientStop { position: 0.1; color: "dark blue" }
                    GradientStop { position: 0.8; color: "blue" }
                }
                border.width: 4
                border.color: "black"
            }
        }
    }
    Column
    {
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.3075
        spacing: parent.width * 0.25

        Repeater
        {
            model: 2

            Rectangle
            {
                id: rowBar
                width: parent.width * 0.95
                height: parent.height * 0.05
                radius: 75
                gradient: Gradient {
                    GradientStop { position: 0.1; color: "dark blue" }
                    GradientStop { position: 0.8; color: "blue" }
                }
                border.width: 4
                border.color: "black"
            }
        }
    }

}
