#-------------------------------------------------
#
# Project created by QtCreator 2015-02-06T19:55:15
#
#-------------------------------------------------

TEMPLATE    = app
TARGET      = scq

VERSION     = 1.0.1
DEFINES     += SCQ_VERSION=\\\"$$VERSION\\\"

CONFIG      += console
CONFIG      -= app_bundle

QT          += core
QT          -= gui

HEADERS += \
    cli.h

SOURCES += main.cpp \
    cli.cpp

TRANSLATIONS += \
   $$PWD/i18n/scq_zh_CN.ts

# Windows assembly message
win32 {
include(scq_assembly.pri)
}

# Qem library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qem/release/ -lqem1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qem/debug/ -lqem1
else:symbian: LIBS += -lqem
else:unix: LIBS += -L$$OUT_PWD/../qem/ -lqem

INCLUDEPATH += $$PWD/../qem/include
DEPENDPATH += $$PWD/../qem
