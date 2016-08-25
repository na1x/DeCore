#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T22:20:18
#
#-------------------------------------------------

CONFIG -= qt

CONFIG+=debug

TARGET = tests
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -Wall -Wextra

INCLUDEPATH +=  include

TEMPLATE = app

LIBS += -lcppunit \
    -lpthread

SOURCES += \
    main.cpp \
    cardTest.cpp \
    engineTest.cpp \
    rulesTest.cpp \
    gameTest.cpp \
    saveRestoreTest.cpp \
    basePlayer.cpp

HEADERS += \
    include/cardTest.h \
    include/engineTest.h \
    include/rulesTest.h \
    include/gameTest.h \
    include/defines.h \
    include/saveRestoreTest.h \
    include/basePlayer.h

INCLUDEPATH += $$PWD/../src/include
DEPENDPATH += $$PWD/../src/include

LIBS += -L$$PWD/.. -ldecore
PRE_TARGETDEPS += $$PWD/../libdecore.a
