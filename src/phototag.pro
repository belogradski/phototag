#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T17:28:55
#
#-------------------------------------------------

QT       += core gui network widgets webenginewidgets webchannel websockets

TARGET = phototag
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += "BUILD_DIR=\"\\\""$$OUT_PWD"\\\"\""

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    geocode_data_manager.cpp \
    webeventlistener.cpp \
    imagelistmodel.cpp \
    exifwrapper.cpp \
    dropablewebview.cpp \
    websocketclientwrapper.cpp \
    websockettransport.cpp \
    sortfilterproxymodel.cpp

HEADERS += \
        mainwindow.h \
    geocode_data_manager.h \
    webeventlistener.h \
    imagelistmodel.h \
    exifwrapper.h \
    dropablewebview.h \
    websocketclientwrapper.h \
    websockettransport.h \
    sortfilterproxymodel.h
FORMS += \
        mainwindow.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    google_maps.html
