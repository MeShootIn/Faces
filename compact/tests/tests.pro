TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/main.cpp

LIBS += \
    -L$$PWD/libs/ -llogger \
    -L$$PWD/libs/ -lvector \
    -L$$PWD/libs/ -lcompact

DISTFILES += \
    libs/compact.dll \
    libs/logger.dll \
    libs/vector.dll

HEADERS += \
    include/ICompact.h \
    include/ILogger.h \
    include/IVector.h \
    include/RC.h
