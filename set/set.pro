QT -= gui

TEMPLATE = lib
DEFINES += SET_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Set.cpp

LIBS += \
    -L$$PWD/libs/ -llogger \
    -L$$PWD/libs/ -lvector

HEADERS += \
    include/ILogger.h \
    include/ISet.h \
    include/IVector.h \
    include/RC.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    libs/logger.dll \
    libs/vector.dll
