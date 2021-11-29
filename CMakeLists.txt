cmake_minimum_required(VERSION 3.5)

project(radar LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_definitions( -DSOFT_VERSION="0.03" )

execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
add_definitions(-DGIT_HASH=\"${GIT_HASH}\")

execute_process(
    COMMAND git log --pretty=format:%cs -1
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_DATE=\"${GIT_DATE}\")

# set(CMAKE_AUTOUIC ON) # 如果ui文件和cpp文件不在同一个目录下，不能使用此项
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/Protocol
    ${CMAKE_SOURCE_DIR}/src/BitColorMap
    ${CMAKE_SOURCE_DIR}/src/UpdateBin
    ${CMAKE_SOURCE_DIR}/src/wave
    ${CMAKE_SOURCE_DIR}/src/motor
    ${CMAKE_SOURCE_DIR}/src/laser
    ${CMAKE_SOURCE_DIR}/src/DevInfo
    ${CMAKE_SOURCE_DIR}/src/saveWave
    ${CMAKE_SOURCE_DIR}/src/ADControll
    ${CMAKE_SOURCE_DIR}/src/DAControll
    ${CMAKE_SOURCE_DIR}/src/gpsInfo
    ${CMAKE_SOURCE_DIR}/src/attitudeSensor
    ${CMAKE_SOURCE_DIR}/src/third_lib/QCustomPlot
    ${CMAKE_SOURCE_DIR}/src/navigation
    )

# set (Project_UI  ${PROJECT_SOURCE_DIR}/mainwindow.ui)
set (Project_UI  
    ${CMAKE_SOURCE_DIR}/ui/mainwindow.ui
    ${CMAKE_SOURCE_DIR}/ui/noteinfo.ui
    ${CMAKE_SOURCE_DIR}/ui/save.ui
    ${CMAKE_SOURCE_DIR}/ui/navigation.ui
    )
qt5_wrap_ui (Project_UI_MOC  ${Project_UI})

set(SOURCES 
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/noteinfo.cpp 
    ${CMAKE_SOURCE_DIR}/save.cpp
    ${CMAKE_SOURCE_DIR}/src/ADControll/ADControl.h
    ${CMAKE_SOURCE_DIR}/src/DAControll/DAControl.h
    ${CMAKE_SOURCE_DIR}/src/gpsInfo/gpsInfo.h
    ${CMAKE_SOURCE_DIR}/src/attitudeSensor/AttitudeSensor.h
    ${CMAKE_SOURCE_DIR}/src/BitColorMap/BitColorMap.cpp
    ${CMAKE_SOURCE_DIR}/src/DevInfo/DevInfo.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/LaserController.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType1.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType2.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType3.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType4.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType5.cpp
    ${CMAKE_SOURCE_DIR}/src/laser/laserType6.cpp
    ${CMAKE_SOURCE_DIR}/src/motor/MotorController.cpp
    ${CMAKE_SOURCE_DIR}/src/motor/Epos2Controller.cpp
    ${CMAKE_SOURCE_DIR}/src/motor/PusiController.cpp
    ${CMAKE_SOURCE_DIR}/src/Protocol/ProtocolDispatch.cpp
    ${CMAKE_SOURCE_DIR}/src/Protocol/ADSampleControl.h
    ${CMAKE_SOURCE_DIR}/src/saveWave/SaveWave.cpp
    ${CMAKE_SOURCE_DIR}/src/third_lib/QCustomPlot/qcustomplot.cpp
    ${CMAKE_SOURCE_DIR}/src/UpdateBin/UpdateBin.cpp
    ${CMAKE_SOURCE_DIR}/src/wave/WaveExtract.cpp
    ${CMAKE_SOURCE_DIR}/src/wave/OfflineWaveform.cpp
    ${CMAKE_SOURCE_DIR}/src/wave/OnlineWaveform.cpp
    ${CMAKE_SOURCE_DIR}/src/navigation/navigation.cpp
    )

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${Project_UI_MOC}
    ${CMAKE_SOURCE_DIR}/res.qrc
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Widgets 
    Qt5::Network 
    Qt5::Core 
    Qt5::Gui
    Qt5::PrintSupport
)

string(TIMESTAMP COMPILE_TIME %Y%m%d_%H%M%S)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_${COMPILE_TIME}_d")
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_${COMPILE_TIME}_r")

