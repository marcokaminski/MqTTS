DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

INCLUDEPATH += $$PWD/ext/qmqtt

PUBLIC_HEADERS += \
    $$PWD/ext/qmqtt/qmqtt_client.h \
    $$PWD/ext/qmqtt/qmqtt_frame.h \
    $$PWD/ext/qmqtt/qmqtt_message.h \
    $$PWD/ext/qmqtt/qmqtt_routesubscription.h \
    $$PWD/ext/qmqtt/qmqtt_routedmessage.h \
    $$PWD/ext/qmqtt/qmqtt_router.h \
    $$PWD/ext/qmqtt/qmqtt_networkinterface.h \
    $$PWD/ext/qmqtt/qmqtt_socketinterface.h \
    $$PWD/ext/qmqtt/qmqtt_timerinterface.h

PRIVATE_HEADERS += \
    $$PWD/ext/qmqtt/qmqtt_client_p.h \
    $$PWD/ext/qmqtt/qmqtt_message_p.h \
    $$PWD/ext/qmqtt/qmqtt_network_p.h \
    $$PWD/ext/qmqtt/qmqtt_socket_p.h \
    $$PWD/ext/qmqtt/qmqtt_ssl_socket_p.h \
    $$PWD/ext/qmqtt/qmqtt_timer_p.h \
    $$PWD/ext/qmqtt/qmqtt_websocket_p.h \
    $$PWD/ext/qmqtt/qmqtt_websocketiodevice_p.h

SOURCES += \
    $$PWD/ext/qmqtt/qmqtt_client_p.cpp \
    $$PWD/ext/qmqtt/qmqtt_client.cpp \
    $$PWD/ext/qmqtt/qmqtt_frame.cpp \
    $$PWD/ext/qmqtt/qmqtt_message.cpp \
    $$PWD/ext/qmqtt/qmqtt_network.cpp \
    $$PWD/ext/qmqtt/qmqtt_routesubscription.cpp \
    $$PWD/ext/qmqtt/qmqtt_router.cpp \
    $$PWD/ext/qmqtt/qmqtt_socket.cpp \
    $$PWD/ext/qmqtt/qmqtt_ssl_socket.cpp \
    $$PWD/ext/qmqtt/qmqtt_timer.cpp \
    $$PWD/ext/qmqtt/qmqtt_websocket.cpp \
    $$PWD/ext/qmqtt/qmqtt_websocketiodevice.cpp
