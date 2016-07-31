#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T22:16:53
#
#-------------------------------------------------

QT       -= core gui

TARGET = decore
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += src/include

SOURCES += \
    src/card.cpp \
    src/cardSet.cpp \
    src/engine.cpp \
    src/round.cpp

HEADERS += \
    src/include/card.h \
    src/include/cardSet.h \
    src/include/engine.h \
    src/include/gameObserver.h \
    src/include/player.h \
    src/include/playerId.h \
    src/include/round.h \
    src/include/rank.h \
    src/include/suit.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
