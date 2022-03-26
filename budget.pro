QT       += core gui charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    addcatdialog.cpp \
    addopdialog.cpp \
    drilldownchart.cpp \
    drilldownslice.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    account.h \
    addcatdialog.h \
    addopdialog.h \
    drilldownchart.h \
    drilldownslice.h \
    initdb.h \
    mainwindow.h

FORMS += \
    account.ui \
    addcatdialog.ui \
    addopdialog.ui \
    mainwindow.ui

TRANSLATIONS += \
    budget_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
