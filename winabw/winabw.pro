#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T18:04:35
#
#-------------------------------------------------

QT       += core gui

TARGET = winabw
TEMPLATE = app

VERSION     = 1.0.0
DEFINES     += WINABW_VERSION=\\\"$$VERSION\\\"


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
    $$PWD/i18n/winabw_zh_CN.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qem/release/ -lqem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qem/debug/ -lqem
else:symbian: LIBS += -lqem
else:unix: LIBS += -L$$OUT_PWD/../qem/ -lqem

INCLUDEPATH += $$PWD/../qem
DEPENDPATH += $$PWD/../qem

RESOURCES += \
    winabw.qrc
