#-------------------------------------------------
#
# Project created by QtCreator 2019-09-27T13:28:12
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += charts


QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Radar
TEMPLATE = app

CONFIG += precompile_header
PRECOMPILED_HEADER  = stable.h

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    noteinfo.cpp \
#    src/ChartView/chartview.cpp \
#    src/Protocol/DoubleWaveProtocol.cpp \
#    src/Protocol/RadarProtocolBasic.cpp \
    src/third_lib/QCustomPlot/qcustomplot.cpp \
    src/ShowWave/getWaveData.cpp \
    src/ShowWave/getWaveShow.cpp \
    save.cpp \
    src/DevInfo/DevInfo.cpp \
    src/UpdateBin/UpdateBin.cpp \
    src/Protocol/ProtocolDispatch.cpp \
    src/Protocol/PreviewProcess.cpp \
    src/laser/LaserController.cpp \
    src/laser/laserType1.cpp \
    src/motor/MotorController.cpp \
    src/motor/Epos2Controller.cpp

HEADERS += \
    stable.h \
        stable.h \
        mainwindow.h \
    noteinfo.h \
    bsp_config.h \
#    src/ChartView/chartview.h \
#    src/Protocol/DoubleWaveProtocol.h \
#    src/Protocol/RadarProtocolBasic.h \
    src/third_lib/QCustomPlot/qcustomplot.h \
    src/ShowWave/getWaveData.h \
    src/ShowWave/getWaveShow.h \
    save.h \
    src/DevInfo/DevInfo.h \
    src/UpdateBin/UpdateBin.h \
    src/Protocol/protocol.h \
    src/Protocol/ProtocolDispatch.h \
    src/Protocol/PreviewProcess.h \
    src/laser/LaserController.h \
    src/laser/laserType1.h \
    src/motor/MotorController.h \
    src/motor/Epos2Controller.h

INCLUDEPATH += ./src/Protocol
INCLUDEPATH += ./src/UpdateBin
INCLUDEPATH += ./src/ShowWave
INCLUDEPATH += ./src/motor
INCLUDEPATH += ./src/laser

FORMS += \
        ui/mainwindow.ui \
    ui/noteinfo.ui \
    ui/save.ui

RESOURCES += \
    res.qrc

VERSION = 1.0.1

RC_ICONS = ./qss/radar_icon.ico

GIT_HASH = $$system(git --git-dir $$PWD/.git log -1 --pretty=format:%h)
DEFINES += GIT_HASH=\\\"$$GIT_HASH\\\"

GIT_DATE = $$system(git --git-dir $$PWD/.git --work-tree $$PWD log -1 --format=%cs )
DEFINES += GIT_DATE=\\\"$$GIT_DATE\\\"

VERSION = 0.02
DEFINES += SOFT_VERSION=\"\\\"$$VERSION\\\"\"

TARGET = Radar_$$VERSION"_"$$GIT_DATE"_"$$GIT_HASH

DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/)

include($$PWD/deploy.pri)
