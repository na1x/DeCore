#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T22:20:18
#
#-------------------------------------------------

QT       -= core

QT       -= gui

TARGET = tests
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH +=  include

TEMPLATE = app

LIBS += -lcppunit

SOURCES += \
    main.cpp \
    cardTest.cpp \
    engineTest.cpp \
    rulesTest.cpp \
    gameTest.cpp

HEADERS += \
    include/cardTest.h \
    include/engineTest.h \
    include/rulesTest.h \
    include/gameTest.h \
    include/defines.h

INCLUDEPATH += $$PWD/../src/include
DEPENDPATH += $$PWD/../src/include

CONFIG(debug, release | debug) {
    LIBS += -L$$PWD/.. -ldecore
    PRE_TARGETDEPS += $$PWD/../libdecore.a
}

CONFIG(release, release | debug) {
    LIBS += -L$$PWD/.. -ldecore
    PRE_TARGETDEPS += $$PWD/../libdecore.a
}
