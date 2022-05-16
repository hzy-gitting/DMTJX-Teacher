QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DEFINES += _HAS_STD_BYTE=0

INCLUDEPATH += \
    C:\Users\hzy\Desktop\MyCode\Teacher\network \
    C:\Users\hzy\Desktop\SDL-devel-1.2.15-VC\SDL-1.2.15\Include \
    C:\Users\hzy\Desktop\ffmpeg_win64_dev\Include

SOURCES += \
    DXGIManager.cpp \
    MonitorMaster.cpp \
    ffmpeg.cpp \
    ffmpegEncoder.cpp \
    filereceiver.cpp \
    main.cpp \
    netmessage.cpp \
    netstudent.cpp \
    network/networkcommunicationsystem.cpp \
    networkmessagelist.cpp \
    networkutils.cpp \
    parametersetttingdialog.cpp \
    rgb2yuv.cpp \
    rtcp.cpp \
    sendfiledialog.cpp \
    sendscreenvideodatathread.cpp \
    systemconfigurationinfo.cpp \
    teachersndmsgwindow.cpp \
    widget.cpp

LIBS +=User32.LIB
LIBS +=gdi32.LIB

LIBS += -LC:\Users\hzy\Desktop\ffmpeg_win64_dev\lib -lavcodec -lavdevice -lavformat -lswscale -lavutil
LIBS += -LC:\Users\hzy\Desktop\SDL-devel-1.2.15-VC\SDL-1.2.15\lib\x64 -lsdl
LIBS+=-ladvapi32
LIBS+=-liphlpapi
LIBS+=-lWs2_32
LIBS+=-lD3D11
LIBS+=-ldxgi
LIBS+=-lgdiplus

HEADERS += \
    DXGIManager.h \
    MonitorMaster.h \
    ffmpeg.h \
    ffmpegEncoder.h \
    filereceiver.h \
    netmessage.h \
    netstudent.h \
    network/networkcommunicationsystem.h \
    networkmessagelist.h \
    networkutils.h \
    parametersetttingdialog.h \
    rgb2yuv.h \
    rtcp.h \
    sendfiledialog.h \
    sendscreenvideodatathread.h \
    systemconfigurationinfo.h \
    teachersndmsgwindow.h \
    widget.h

FORMS += \
    parametersetttingdialog.ui \
    sendfiledialog.ui \
    teachersndmsgwindow.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
