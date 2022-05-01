QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += \
    C:\Users\hzy\Desktop\MyCode\Teacher\network \
    C:\Users\hzy\Desktop\SDL-devel-1.2.15-VC\SDL-1.2.15\Include \
    C:\Users\hzy\Desktop\ffmpeg_win64_dev\Include

SOURCES += \
    MonitorMaster.cpp \
    ffmpeg.cpp \
    ffmpegEncoder.cpp \
    filereceiver.cpp \
    main.cpp \
    netstudent.cpp \
    network/networkcommunicationsystem.cpp \
    networkutils.cpp \
    parametersetttingdialog.cpp \
    rgb2yuv.cpp \
    rtcp.cpp \
    sendfiledialog.cpp \
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

HEADERS += \
    MonitorMaster.h \
    ffmpeg.h \
    ffmpegEncoder.h \
    filereceiver.h \
    netstudent.h \
    network/networkcommunicationsystem.h \
    networkutils.h \
    parametersetttingdialog.h \
    rgb2yuv.h \
    rtcp.h \
    sendfiledialog.h \
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
