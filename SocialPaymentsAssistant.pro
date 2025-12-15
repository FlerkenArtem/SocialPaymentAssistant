QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    applicantreg.cpp \
    branchofficeslist.cpp \
    empmainwindow.cpp \
    empreg.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    newapplication.cpp \
    registrationwidget.cpp

HEADERS += \
    about.h \
    applicantreg.h \
    branchofficeslist.h \
    empmainwindow.h \
    empreg.h \
    loginwidget.h \
    mainwindow.h \
    newapplication.h \
    passportdata.h \
    registrationwidget.h

FORMS += \
    about.ui \
    applicantreg.ui \
    branchofficeslist.ui \
    empmainwindow.ui \
    empreg.ui \
    loginwidget.ui \
    mainwindow.ui \
    newapplication.ui \
    registrationwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
