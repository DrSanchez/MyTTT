import QtQuick 2.0

Rectangle
{
    id: gameScreenContainer
    width: parent.width
    height: parent.height


    Grid
    {
        id: boardView
        width: parent.width
        height: parent.height * 0.66
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        columns: 3

        Repeater
        {
            model: 9

            Tile
            {
                width: boardView.width
                height: boardView.height

                onClicked:
                {
                    if (Client.validateMove((index % 3), (index - x) / 3))
                    {

                    }
                }
            }
        }
    }

}
