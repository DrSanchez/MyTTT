import QtQuick 2.0
import QtGraphicalEffects 1.0

Item
{
    id: tileContainer
    state: ""

    signal activated

    Image
    {
        id: tileImage
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
    }
    DropShadow
    {
        id: shadow
        anchors.fill: tileImage
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8.0
        samples: 16
        color: "#80000000"
        source: tileImage
    }

    states: [
        State
        {
            name: ""
            PropertyChanges
            {
                target: tileImage
                source: ""
                width: tileContainer.width
            }
        },
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
        onClicked: tileContainer.activated();
    }
}
