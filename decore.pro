#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T22:16:53
#
#-------------------------------------------------

CONFIG -= qt

CONFIG+=debug

TARGET = decore
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -Wall -Wextra

INCLUDEPATH += src/include

SOURCES += \
    src/card.cpp \
    src/cardSet.cpp \
    src/engine.cpp \
    src/rules.cpp \
    src/deck.cpp \
    src/gameCardsTracker.cpp \
    src/dataWriter.cpp \
    src/dataReader.cpp

HEADERS += \
    src/include/card.h \
    src/include/cardSet.h \
    src/include/engine.h \
    src/include/gameObserver.h \
    src/include/player.h \
    src/include/playerId.h \
    src/include/rank.h \
    src/include/suit.h \
    src/include/rules.h \
    src/include/deck.h \
    src/include/gameCardsTracker.h \
    src/include/dataWriter.h \
    src/include/dataReader.h
