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
    src/Protocol/AdSampleControl.cpp \
    src/laser/laserType3.cpp \
    src/laser/laserType4.cpp \
    src/laser/laserType5.cpp \
    src/laser/laserType6.cpp \
    src/saveWave/SaveWave.cpp \
    src/wave/OfflineWaveform.cpp \
    src/wave/OnlineWaveform.cpp \
    src/wave/WaveExtract.cpp \
    src/laser/laserType2.cpp \
    src/third_lib/QCustomPlot/qcustomplot.cpp \
    save.cpp \
    src/DevInfo/DevInfo.cpp \
    src/UpdateBin/UpdateBin.cpp \
    src/Protocol/ProtocolDispatch.cpp \
    src/laser/LaserController.cpp \
    src/laser/laserType1.cpp \
    src/motor/MotorController.cpp \
    src/motor/Epos2Controller.cpp

HEADERS += \
    common.h \
    src/ADControll/ADControl.h \
    src/DAControll/DAControl.h \
    src/Protocol/AdSampleControl.h \
    src/attitudeSensor/AttitudeSensor.h \
    src/gpsInfo/gpsInfo.h \
    src/laser/laserType3.h \
    src/laser/laserType4.h \
    src/laser/laserType5.h \
    src/laser/laserType6.h \
    src/saveWave/SaveWave.h \
    src/wave/OfflineWaveform.h \
    src/wave/OnlineWaveform.h \
    src/wave/WaveExtract.h \
    src/laser/laserType2.h \
        mainwindow.h \
    noteinfo.h \
    bsp_config.h \
    src/third_lib/QCustomPlot/qcustomplot.h \
    save.h \
    src/DevInfo/DevInfo.h \
    src/UpdateBin/UpdateBin.h \
    src/Protocol/protocol.h \
    src/Protocol/ProtocolDispatch.h \
    src/laser/LaserController.h \
    src/laser/laserType1.h \
    src/motor/MotorController.h \
    src/motor/Epos2Controller.h

INCLUDEPATH += ./src/Protocol
INCLUDEPATH += ./src/UpdateBin
INCLUDEPATH += ./src/wave
INCLUDEPATH += ./src/motor
INCLUDEPATH += ./src/laser
INCLUDEPATH += ./src/DevInfo
INCLUDEPATH += ./src/saveWave
INCLUDEPATH += ./src/ADControll
INCLUDEPATH += ./src/DAControll
INCLUDEPATH += ./src/gpsInfo
INCLUDEPATH += ./src/attitudeSensor
INCLUDEPATH += ./src/third_lib/QCustomPlot/


FORMS += \
        ui/mainwindow.ui \
    ui/noteinfo.ui \
    ui/save.ui

RESOURCES += \
    res.qrc

RC_ICONS = ./qss/radar_icon.ico

GIT_HASH = $$system(git --git-dir $$PWD/.git log -1 --pretty=format:%h)
DEFINES += GIT_HASH=\\\"$$GIT_HASH\\\"

GIT_DATE = $$system(git --git-dir $$PWD/.git --work-tree $$PWD log -1 --format=%cs )
DEFINES += GIT_DATE=\\\"$$GIT_DATE\\\"

VERSION = 0.115
DEFINES += SOFT_VERSION=\"\\\"$$VERSION\\\"\"

#TARGET = Radar_$$VERSION"_"$$GIT_DATE"_"$$GIT_HASH
TARGET = Radar_$$VERSION

#DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/bin/)

#include($$PWD/deploy.pri)
