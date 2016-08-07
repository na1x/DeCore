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
    rulesTest.cpp

HEADERS += \
    include/cardTest.h \
    include/engineTest.h \
    include/rulesTest.h

INCLUDEPATH += $$PWD/../src/include
DEPENDPATH += $$PWD/../src/include

CONFIG(debug, release | debug) {
    LIBS += -L$$PWD/../../build-decore-debug/ -ldecore
    PRE_TARGETDEPS += $$PWD/../../build-decore-debug/libdecore.a
}

CONFIG(release, release | debug) {
    LIBS += -L$$PWD/../../build-decore-release/ -ldecore
    PRE_TARGETDEPS += $$PWD/../../build-decore-release/libdecore.a
}
