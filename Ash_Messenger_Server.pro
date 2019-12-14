QT -= gui
QT += core
QT += sql
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/Database/database.cpp \
        src/Messaging/Chat/chatsystem.cpp \
        src/Messaging/User/searchchatuser.cpp \
        src/Messaging/User/userauthentication.cpp \
        src/Network/DataTransferProtocolAsh/dtpa.cpp \
        src/Network/DataTransferProtocolAsh/dtpaform.cpp \
        src/Network/DataTransferProtocolAsh/dtpareceiver.cpp \
        src/Network/DataTransferProtocolAsh/dtpareceivermanager.cpp \
        src/Network/DataTransferProtocolAsh/dtparequest.cpp \
        src/Network/DataTransferProtocolAsh/dtpasender.cpp \
        src/Network/SSL/sslconnection.cpp \
        src/Network/SSL/sslconnectionpool.cpp \
        src/Network/SSL/sslserver.cpp \
        src/Network/connectionsmanager.cpp \
        src/Network/dtpasenderpool.cpp \
        src/Other/config.cpp \
        src/main.cpp \
        src/performancebenchmark.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

release: DESTDIR = build/release
debug:   DESTDIR = build/debug
profile:   DESTDIR = build/profile

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

HEADERS += \
    src/Database/database.h \
    src/Messaging/Chat/chatsystem.h \
    src/Messaging/User/searchchatuser.h \
    src/Messaging/User/userauthentication.h \
    src/Network/DataTransferProtocolAsh/dtpa.h \
    src/Network/DataTransferProtocolAsh/dtpaform.h \
    src/Network/DataTransferProtocolAsh/dtpareceiver.h \
    src/Network/DataTransferProtocolAsh/dtpareceivermanager.h \
    src/Network/DataTransferProtocolAsh/dtparequest.h \
    src/Network/DataTransferProtocolAsh/dtpasender.h \
    src/Network/SSL/sslconnection.h \
    src/Network/SSL/sslconnectionpool.h \
    src/Network/SSL/sslserver.h \
    src/Network/connectionsmanager.h \
    src/Network/dtpasenderpool.h \
    src/Other/config.h \
    src/performancebenchmark.h

DISTFILES += \
    resources/Config/config.json \
    resources/Database/credentials.txt \
    resources/Database/defaultTables.sql \
    resources/SSL/server.crt \
    resources/SSL/server.csr \
    resources/SSL/server.key

RESOURCES = resources.qrc
