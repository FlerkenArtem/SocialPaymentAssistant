QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    addressedit.cpp \
    adoptationcertificateedit.cpp \
    applicantdatainsert.cpp \
    bankedit.cpp \
    branchofficeslist.cpp \
    empdatainsert.cpp \
    empmainwindow.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    newapplication.cpp \
    passportedit.cpp \
    policyedit.cpp \
    registrationwidget.cpp \
    snilsedit.cpp

HEADERS += \
    about.h \
    addressedit.h \
    adoptationcertificateedit.h \
    applicantdatainsert.h \
    bankedit.h \
    branchofficeslist.h \
    empdatainsert.h \
    empmainwindow.h \
    loginwidget.h \
    mainwindow.h \
    newapplication.h \
    passportdata.h \
    passportedit.h \
    policyedit.h \
    registrationwidget.h \
    snilsedit.h

FORMS += \
    about.ui \
    addressedit.ui \
    adoptationcertificateedit.ui \
    applicantdatainsert.ui \
    bankedit.ui \
    branchofficeslist.ui \
    empdatainsert.ui \
    empmainwindow.ui \
    loginwidget.ui \
    mainwindow.ui \
    newapplication.ui \
    passportedit.ui \
    policyedit.ui \
    registrationwidget.ui \
    snilsedit.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
