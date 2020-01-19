#-------------------------------------------------
#
# Project created by QtCreator 2020-01-02T13:44:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = audio
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

CONFIG += c++11 \
          no_keywords link_pkgconfig

PKGCONFIG += libmatemixer \
             Qt5Xdg \
             gio-2.0 \
             libxml-2.0 \
             Qt5Multimedia

SOURCES += \
        main.cpp \
        utils.cpp \
    ukmedia_control_center_widget.cpp
#    appvolume.cpp

HEADERS += \
        utils.h \
        ukmedia_control_center_widget.h
#    appvolume.h

FORMS += \
        widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += /usr/lib/x86_64-linux-gnu/libgio-2.0.so
INCLUDEPATH +=  /usr/include/glib-2.0/

TRANSLATIONS += translations/audio-zh_CN.ts

DISTFILES +=

RESOURCES += \
    audio.qrc
