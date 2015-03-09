#-------------------------------------------------
#
# Project created by QtCreator 2015-02-03T19:34:25
#
#-------------------------------------------------

TARGET = qem
TEMPLATE = lib

QT       += xml
QT       -= gui

# Qem version
VERSION = 1.2.6
DEFINES += QEM_VERSION=\\\"$$VERSION\\\"


# You'll need to define this one manually if using a build system other
# than qmake or using QuaZIP sources directly in your project.
CONFIG(staticlib): DEFINES += QEM_STATIC

# This one handles dllimport/dllexport directives.
DEFINES += QEM_BUILD

# This defined for enable QML property, enums and invokable function.
DEFINES += QEM_QML_TARGET

# Input
include(qem.pri)

unix:!symbian {
    headers.path=$$PREFIX/include/qem
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib/$${LIB_ARCH}
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
    INSTALLS += headers target
}

win32 {
    headers.path=$$PREFIX/include/qem
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target
    # workaround for qdatetime.h macro bug
    DEFINES += NOMINMAX

    # assembly message
    include(qem_assembly.pri)
}

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3DB2F4C
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = qem.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles

    vendorinfo += "%{\"PW\"}" ":\"PW\""
    my_deployment.pkg_prerules += vendorinfo
    DEPLOYMENT += my_deployment

    # Symbian have a different syntax
    DEFINES -= QEM_VERSION=\\\"$$VERSION\\\"
    DEFINES += QEM_VERSION=\"$$VERSION\"

    # for QEM static library
#    CONFIG += staticlib
#    CONFIG += debug_and_release

#    LIBS += -lezip

#    Export headers to SDK Epoc32/include directory
#    exportheaders.sources = $$HEADERS
#    exportheaders.path = qem
#    for(header, exportheaders.sources) {
#        BLD_INF_RULES.prj_exports += "$$header $$exportheaders.path/$$basename(header)"
#    }
}
