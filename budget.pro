QT       += core gui charts sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = CashCash

INCLUDEPATH = includes/ sources/ resources/ resources/uiFiles resources/translation

SOURCES += \
    sources/aboutdialog.cpp \
    sources/groupinputdialog.cpp \
    sources/rulesdialog.cpp \
    sources/ruleslist.cpp \
    sources/account.cpp \
    sources/addopdialog.cpp \
    sources/catdialog.cpp \
    sources/catslist.cpp \
    sources/chartsview.cpp \
    sources/coloredprogressbar.cpp \
    sources/csveditor.cpp \
    sources/csvimporterwizard.cpp \
    sources/customslice.cpp \
    sources/filterswidget.cpp \
    sources/goaldialog.cpp \
    sources/goalsview.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/newaccountdialog.cpp \
    sources/operationsview.cpp \
    sources/statswidget.cpp \
    sources/utilities.cpp \
    sources/welcomedialog.cpp \
    sources/tablewidget.cpp \
    sources/sqlrelationalmodel.cpp

HEADERS += \
    includes/aboutdialog.h \
    includes/groupinputdialog.h \
    includes/account.h \
    includes/addopdialog.h \
    includes/catdialog.h \
    includes/catslist.h \
    includes/chartsview.h \
    includes/coloredprogressbar.h \
    includes/csveditor.h \
    includes/csvimporterwizard.h \
    includes/customslice.h \
    includes/defines.h \
    includes/filterswidget.h \
    includes/goaldialog.h \
    includes/goalsview.h \
    includes/initdb.h \
    includes/mainwindow.h \
    includes/newaccountdialog.h \
    includes/operationsview.h \
    includes/statswidget.h \
    includes/utilities.h \
    includes/welcomedialog.h \
    includes/rulesdialog.h \
    includes/ruleslist.h \
    includes/tablewidget.h \
    includes/sqlrelationalmodel.h

FORMS += \
    resources/uiFiles/aboutdialog.ui \
    resources/uiFiles/fieldspage.ui \
    resources/uiFiles/linepage.ui \
    resources/uiFiles/groupinputdialog.ui \
    resources/uiFiles/catslist.ui \
    resources/uiFiles/addopdialog.ui \
    resources/uiFiles/goaldialog.ui \
    resources/uiFiles/mainwindow.ui \
    resources/uiFiles/newaccountdialog.ui \
    resources/uiFiles/statswidget.ui \
    resources/uiFiles/rulesdialog.ui \
    resources/uiFiles/tablewidget.ui

TRANSLATIONS += \
    resources/translation/budget_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/budget.qrc

RC_ICONS = resources/images/euro_logo.ico
