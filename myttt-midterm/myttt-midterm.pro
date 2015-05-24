# Add more folders to ship with the application, here
#folder_01.source = qml/
#folder_01.target = qml
#DEPLOYMENTFOLDERS = folder_01

# Adds Cx11 support, e.g. nullptr...
CONFIG   += c++11 (Qt5)

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    tttclient.cpp \
    tttuser.cpp \
    gamehandler.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    tttclient.h \
    tttuser.h \
    gamehandler.h

INCLUDEPATH += $${_PRO_FILE_PWD_}/../myttt-common/

OTHER_FILES += qml/main.qml \
    qml/CustomTextEdit.qml \
    qml/GameScreen.qml \
    qml/GeneralButton.qml \
    qml/InfoDialog.qml \
    qml/LobbyScreen.qml \
    qml/StartScreen.qml \
    qml/UserItem.qml \
    qml/Tile.qml

RESOURCES += \
    images.qrc
