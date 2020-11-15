QT -= gui
QT += core network websockets

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mqtts.cpp

#include(mqtt.pri)
#HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ext/qmqtt/ -lQt5Qmqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ext/qmqtt/ -lQt5Qmqttd
else:unix: LIBS += -lQt5Qmqtt

INCLUDEPATH += $$PWD/ext/qmqtt/inc
DEPENDPATH += $$PWD/ext/qmqtt/inc

HEADERS += \
    mqtts.h
