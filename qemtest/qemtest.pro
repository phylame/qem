#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T10:49:16
#
#-------------------------------------------------

TEMPLATE = app

QT       += testlib

QT       -= gui

TARGET   = qemtest
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += \
   ../3rdparty/zlib \
   ../3rdparty/quazip

HEADERS += \
    testpart.h \
    testattributes.h \
    testreadandmake.h \
    testfileobject.h \
    testtextobject.h

SOURCES += \
    testpart.cpp \
    testattributes.cpp \
    testqem.cpp \
    testreadandmake.cpp \
    testfileobject.cpp \
    testtextobject.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qem/release/ -lqem1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qem/debug/ -lqem1
else:symbian: LIBS += -lqem
else:unix: LIBS += -L$$OUT_PWD/../qem/ -lqem

INCLUDEPATH += $$PWD/../qem/include
DEPENDPATH += $$PWD/../qem

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/quazip/debug/ -lquazip
else:symbian: LIBS += -lquazip
else:unix: LIBS += -L$$OUT_PWD/../3rdparty/quazip/ -lquazip

INCLUDEPATH += $$PWD/../3rdparty/quazip
DEPENDPATH += $$PWD/../3rdparty/quazip

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/release/quazip.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/debug/quazip.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/libquazip.a
