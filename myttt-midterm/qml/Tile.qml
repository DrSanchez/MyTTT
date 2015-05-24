import QtQuick 2.0

Item
{
    id: tileContainer
    state: ""

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
                width: tileContainer.width * 0.785
            }
        },
        State
        {
            name: "O"
            PropertyChanges
            {
                target: tileImage
                source: "../images/o_piece.png"
                width: tileContainer.width * 0.725
            }
        }
    ]

    MouseArea
    {
        anchors.fill: parent
        onClicked: tileContainer.clicked()
    }
}
