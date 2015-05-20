#include <QQmlEngine>
#include <QQmlContext>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

//user objects
#include "tttclient.h"

int main(int argc, char *argv[])
{
    int screenWidth = 0;
    int screenHeight = 0;
    bool showFullscreen = false;

    QGuiApplication app(argc, argv);
    QtQuick2ApplicationViewer viewer;
    QScreen * screen = app.primaryScreen();

    if (showFullscreen)
    {
        screenWidth = screen->size().width();
        screenHeight = screen->size().height();
    }
    else if (!showFullscreen)
    {
        screenWidth = screen->availableGeometry().width();
        screenHeight = screen->availableGeometry().height();
    }

    TTTUser * user = new TTTUser();
    TTTClient * client = new TTTClient(user);


    viewer.engine()->rootContext()->setContextProperty("Client", client);

    //expose properties to qml
    viewer.engine()->rootContext()->setContextProperty("ShowFullscreen", showFullscreen);
    viewer.engine()->rootContext()->setContextProperty("ScreenWidth", screenWidth);
    viewer.engine()->rootContext()->setContextProperty("ScreenHeight", screenHeight);

    viewer.setMainQmlFile(QStringLiteral("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
