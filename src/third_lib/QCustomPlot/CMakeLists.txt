cmake_minimum_required(VERSION 3.10)

#project(qcustomplot LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

aux_source_directory(. SRC_LIST)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/static_lib) #静态库位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dynamic_lib) #动态库位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) #可执行文件位置, 等效EXECUTABLE_OUTPUT_PATH

#add_library(qcustomplot_static STATIC
#${SRC_LIST }
#)
#set_target_properties(qcustomplot_static PROPERTIES AUTOMOC TRUE)
#target_link_libraries(qcustomplot_static
#Qt5::Widgets
#Qt5::PrintSupport
#)

add_library(qcustomplot_shared SHARED
    ${SRC_LIST} 
    ${APP_ICON_RESOURCE_WINDOWS})

set_target_properties(qcustomplot_shared PROPERTIES AUTOMOC TRUE)
target_link_libraries(qcustomplot_shared
    Qt5::Widgets
    Qt5::PrintSupport)
