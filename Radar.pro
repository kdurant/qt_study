#-------------------------------------------------
#
# Project created by QtCreator 2019-09-27T13:28:12
#
#-------------------------------------------------

QT       += core gui
QT       += network


QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Radar
TEMPLATE = app

VERSION = 0.125
DEFINES += SOFT_VERSION=\"\\\"$$VERSION\\\"\"

# DEFINES += DEBUG_WATER_GUARD

#DEFINES += QT_NO_DEBUG_OUTPUT


include(./src/Protocol/protocol.pri)
include(./src/BitColorMap/BitColorMap.pri)
include(./src/motor/MotorController.pri)
include(./src/laser/LaserController.pri)
include(./src/navigation/navigation.pri)
include(./src/UpdateBin/UpdateBin.pri)
include(./src/third_lib/QCustomPlot/qcustomplot.pri)

INCLUDEPATH += ./src/UpdateBin
INCLUDEPATH += ./src/wave
INCLUDEPATH += ./src/DevInfo
INCLUDEPATH += ./src/saveWave
INCLUDEPATH += ./src/ADControll
INCLUDEPATH += ./src/DAControll
INCLUDEPATH += ./src/gpsInfo
INCLUDEPATH += ./src/attitudeSensor
INCLUDEPATH += ./src/navigation
INCLUDEPATH += ./src/third_lib/QCustomPlot


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    noteinfo.cpp \
    src/saveWave/SaveWave.cpp \
    src/wave/OfflineWaveform.cpp \
    src/wave/OnlineWaveform.cpp \
    src/wave/WaveExtract.cpp \
    save.cpp \
    src/DevInfo/DevInfo.cpp \
    src/UpdateBin/UpdateBin.cpp \

HEADERS += \
    common.h \
    src/ADControll/ADControl.h \
    src/DAControll/DAControl.h \
    src/attitudeSensor/AttitudeSensor.h \
    src/gpsInfo/gpsInfo.h \
    src/saveWave/SaveWave.h \
    src/wave/OfflineWaveform.h \
    src/wave/OnlineWaveform.h \
    src/wave/WaveExtract.h \
        mainwindow.h \
    noteinfo.h \
    bsp_config.h \
    save.h \
    src/DevInfo/DevInfo.h \
    src/UpdateBin/UpdateBin.h \


FORMS += \
        ui/mainwindow.ui \
        ui/noteinfo.ui \
        ui/navigation.ui \
        ui/save.ui

RESOURCES += \
    res.qrc

RC_ICONS = ./qss/radar_icon.ico

GIT_HASH = $$system(git --git-dir $$PWD/.git log -1 --pretty=format:%h)
DEFINES += GIT_HASH=\\\"$$GIT_HASH\\\"

GIT_DATE = $$system(git --git-dir $$PWD/.git --work-tree $$PWD log -1 --format=%cs )
DEFINES += GIT_DATE=\\\"$$GIT_DATE\\\"


Debug: TARGET = Radar_$$VERSION"_"$$GIT_DATE"_"$$GIT_HASH"_debug"
Release: TARGET = Radar_$$VERSION"_"$$GIT_DATE"_"$$GIT_HASH"_release"
#TARGET = Radar_$$VERSION

DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/)

#include($$PWD/deploy.pri)
