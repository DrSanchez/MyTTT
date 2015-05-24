import QtQuick 2.0

Item
{
    id: tileContainer
    visible: false

    signal clicked

    Image
    {
        id: tileImage
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
    }

    states: [
        State
        {
            name: "X"
            PropertyChanges
            {
                target: tileImage
                source: "../images/x_piece.png"
            }
        },
        State
        {
            name: "O"
            PropertyChanges
            {
                target: tileImage
                source: "../images/o_piece.png"
            }
        }
    ]

    MouseArea
    {
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}
