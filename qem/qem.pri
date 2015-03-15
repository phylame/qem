
INCLUDEPATH += \
    ../3rdparty/zlib \
    ../3rdparty/quazip \
    include

!win32: !symbian: LIBS += -lz
win32 {
    # workaround for qdatetime.h macro bug
    DEFINES += NOMINMAX
}

HEADERS += \
    include/textobject.h \
    include/qem_global.h \
    include/qem.h \
    include/part.h \
    include/fileutils.h \
    include/fileobject.h \
    include/filefactory.h \
    include/chapter.h \
    include/book.h \
    include/attributes.h \
    include/formats/umd.h \
    include/formats/txt.h \
    include/formats/pmab.h \
    include/formats/jar.h \
    include/formats/all.h \
    include/formats/epub.h \
    src/formats/epub/writer.h \
    $$PWD/include/utils.h

SOURCES += \
    src/textobject.cpp \
    src/qem.cpp \
    src/part.cpp \
    src/fileutils.cpp \
    src/fileobject.cpp \
    src/filefactory.cpp \
    src/chapter.cpp \
    src/book.cpp \
    src/attributes.cpp \
    src/formats/umd.cpp \
    src/formats/txt.cpp \
    src/formats/pmab.cpp \
    src/formats/jar.cpp \
    src/formats/epub.cpp \
    src/formats/epub/writer.cpp \
    $$PWD/src/utils.cpp

RESOURCES += \
    $$PWD/qem.qrc

TRANSLATIONS += \
    $$PWD/i18n/qem_zh_CN.ts

## Qt 5.x not contains zlib
#contains(QT_MAJOR_VERSION, 5) {
#SOURCES += \
#    ../3rdparty/zlib/alder32.c \
#    ../3rdparty/zlib/compress.c \
#    ../3rdparty/zlib/crc32.c \
#    ../3rdparty/zlib/deflate.c \
#    ../3rdparty/zlib/inflate.c
#}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/quazip/debug/ -lquazip
else:symbian: LIBS += -lquazip
else:unix: LIBS += -L$$OUT_PWD/../3rdparty/quazip/ -lquazip

INCLUDEPATH += $$PWD/../3rdparty/quazip
DEPENDPATH += $$PWD/../3rdparty/quazip

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/release/quazip.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/debug/quazip.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/libquazip.a
