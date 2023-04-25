#-------------------------------------------------
#
# Project created by QtCreator 2019-09-27T13:28:12
#
#-------------------------------------------------

QT       += core gui
QT       += network
TARGET = Radar_release
#Debug: TARGET = Radar_release
#Release: TARGET = Radar_release

QMAKE_CXXFLAGS += -std=c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TEMPLATE = app

VERSION = 0.131
DEFINES += SOFT_VERSION=\"\\\"$$VERSION\\\"\"

# DEFINES += DEBUG_WATER_GUARD

#DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += \
        ./src/ \
        ./src/third_lib


include(./src/common/common.pri)
include(./src/Protocol/protocol.pri)
include(./src/BitColorMap/BitColorMap.pri)
include(./src/motor/MotorController.pri)
include(./src/laser/LaserController.pri)
include(./src/navigation/navigation.pri)
include(./src/UpdateBin/UpdateBin.pri)
include(./src/wave/wave.pri)
include(./src/saveWave/ReadHardDisk.pri)
include(./src/sensors/sensors.pri)
include(./src/DevInfo/DevInfo.pri)
include(./src/third_lib/QCustomPlot/qcustomplot.pri)
include(./src/NoteInfo/NoteInfo.pri)
include(./src/mainwindow/mainwindow.pri)
include(./src/RadarWidget/radarwidget.pri)

RESOURCES += \
    res.qrc

RC_ICONS = ./qss/radar_icon.ico

GIT_HASH = $$system(git --git-dir $$PWD/.git log -1 --pretty=format:%h)
DEFINES += GIT_HASH=\\\"$$GIT_HASH\\\"

GIT_DATE = $$system(git --git-dir $$PWD/.git --work-tree $$PWD log -1 --format=%cs )
DEFINES += GIT_DATE=\\\"$$GIT_DATE\\\"



unix: DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/)

#include($$PWD/deploy.pri)
