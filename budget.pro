QT       += core gui charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    addopdialog.cpp \
    catdialog.cpp \
    catslist.cpp \
    chartsview.cpp \
    drilldownchart.cpp \
    drilldownslice.cpp \
    filtersdialog.cpp \
    goalsview.cpp \
    main.cpp \
    mainwindow.cpp \
    operationsview.cpp \
    welcomedialog.cpp

HEADERS += \
    account.h \
    addopdialog.h \
    catdialog.h \
    catslist.h \
    chartsview.h \
    drilldownchart.h \
    drilldownslice.h \
    filtersdialog.h \
    goalsview.h \
    initdb.h \
    mainwindow.h \
    operationsview.h \
    welcomedialog.h

FORMS += \
    addopdialog.ui \
    filtersdialog.ui \
    mainwindow.ui

TRANSLATIONS += \
    budget_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    budget.qrc
