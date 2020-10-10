TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/main.cpp

LIBS += \
    -L$$PWD/libs/ -llogger \
    -L$$PWD/libs/ -lvector \
    -L$$PWD/libs/ -lset

DISTFILES += \
    libs/logger.dll \
    libs/set.dll \
    libs/vector.dll

HEADERS += \
    include/ILogger.h \
    include/ISet.h \
    include/IVector.h \
    include/RC.h
