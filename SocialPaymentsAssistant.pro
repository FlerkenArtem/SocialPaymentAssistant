QT       += core gui sql printsupport testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    acceptedapplicationsmodel.cpp \
    applicantreg.cpp \
    applicationsproxymodel.cpp \
    branchofficeslist.cpp \
    branchofficeslistmodel.cpp \
    branchofficesproxymodel.cpp \
    empapplicationsmodel.cpp \
    empmainwindow.cpp \
    empreg.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    myapplicationsmodel.cpp \
    newapplication.cpp \
    pdfdocumentgenerator.cpp \
    registrationwidget.cpp \
    tests/testempregmodule.cpp \
    tests/testintegration.cpp \
    tests/testload.cpp \
    tests/testloginwidgetblackbox.cpp \
    tests/testloginwidgetwhitebox.cpp

HEADERS += \
    about.h \
    acceptedapplicationsmodel.h \
    applicantreg.h \
    applicationsproxymodel.h \
    branchofficeslist.h \
    branchofficeslistmodel.h \
    branchofficesproxymodel.h \
    empapplicationsmodel.h \
    empmainwindow.h \
    empreg.h \
    loginwidget.h \
    mainwindow.h \
    myapplicationsmodel.h \
    newapplication.h \
    pdfdocumentgenerator.h \
    registrationwidget.h \
    tests/testempregmodule.h \
    tests/testintegration.h \
    tests/testload.h \
    tests/testloginwidgetblackbox.h \
    tests/testloginwidgetwhitebox.h

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

RESOURCES +=

DISTFILES += \
    README.md \
    social_payments_assistant.sql
