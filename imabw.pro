TEMPLATE = subdirs

SUBDIRS += \
    3rdparty/quazip \
    qem \
    qemtest \
    scq \
#    symabw
    winabw

#qem.depends = quazip
#qemtest.depends = qem
#scq.depends = qem
#imabw.depends = qem
