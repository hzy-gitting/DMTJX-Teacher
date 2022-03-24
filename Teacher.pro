QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += \
    C:\Users\hzy\Desktop\MyCode\Teacher\network
SOURCES += \
    main.cpp \
    messageform.cpp \
    network/networkcommunicationsystem.cpp \
    widget.cpp
LIBS +=User32.LIB

HEADERS += \
    messageform.h \
    network/networkcommunicationsystem.h \
    widget.h

FORMS += \
    messageform.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
