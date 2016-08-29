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

INCLUDEPATH += include

SOURCES += \
    card.cpp \
    cardSet.cpp \
    engine.cpp \
    rules.cpp \
    deck.cpp \
    gameCardsTracker.cpp \
    dataWriter.cpp \
    dataReader.cpp

HEADERS += \
    include/card.h \
    include/cardSet.h \
    include/engine.h \
    include/gameObserver.h \
    include/player.h \
    include/playerId.h \
    include/rank.h \
    include/suit.h \
    include/rules.h \
    include/deck.h \
    include/gameCardsTracker.h \
    include/dataWriter.h \
    include/dataReader.h
