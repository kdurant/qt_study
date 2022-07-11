# cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=DEBUG | cmake --build build
cmake_minimum_required(VERSION 3.10)

project(radar LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (UNIX)
    set(CMAKE_PREFIX_PATH "/opt/Qt5.12.0/5.12.0/gcc_64")
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_definitions( -DSOFT_VERSION="0.125" )

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

add_subdirectory(third_lib/QCustomPlot)

set(CMAKE_AUTOUIC_SEARCH_PATHS ../ui)
set(CMAKE_AUTOUIC ON) # 如果ui文件和cpp文件不在同一个目录下，不能使用此项
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS ../qss/app.rc)

include_directories(
    common
    Protocol
    BitColorMap
    UpdateBin
    wave
    motor
    laser
    DevInfo
    saveWave
    sensors
    third_lib/QCustomPlot
    navigation
    NoteInfo
    mainwindow
    )

# set (Project_UI  ${PROJECT_SOURCE_DIR}/mainwindow.ui)
set (Project_UI  
    ../ui/mainwindow.ui
    ../ui/noteinfo.ui
    ../ui/navigation.ui
    )
qt5_wrap_ui (Project_UI_MOC  ${Project_UI})

set(SOURCES 
    mainwindow/main.cpp
    mainwindow/mainwindow.cpp
    NoteInfo/noteinfo.cpp 
    sensors/ADControl.h
    sensors/DAControl.h
    sensors/gpsInfo.h
    sensors/AttitudeSensor.h
    BitColorMap/BitColorMap.cpp
    DevInfo/DevInfo.cpp
    laser/LaserController.cpp
    laser/laserType1.cpp
    laser/laserType2.cpp
    laser/laserType3.cpp
    laser/laserType4.cpp
    laser/laserType5.cpp
    laser/laserType6.cpp
    motor/MotorController.cpp
    motor/Epos2Controller.cpp
    motor/PusiController.cpp
    Protocol/ProtocolDispatch.cpp
    Protocol/AdSampleControl.h
    saveWave/SaveWave.cpp
    UpdateBin/UpdateBin.cpp
    wave/WaveExtract.cpp
    wave/OfflineWaveform.cpp
    wave/OnlineWaveform.cpp
    navigation/navigation.cpp
    navigation/LineWithArrow.cpp
    navigation/MapView.cpp
    )

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${Project_UI_MOC}
    ../res.qrc
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Widgets 
    Qt5::Network 
    Qt5::Core 
    Qt5::Gui
    Qt5::PrintSupport
    qcustomplot_shared
)

string(TIMESTAMP COMPILE_TIME %Y%m%d_%H%M%S)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_${COMPILE_TIME}_d")
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_${COMPILE_TIME}_r")

