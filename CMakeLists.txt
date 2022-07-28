cmake_minimum_required(VERSION 3.10)
set(projectName radar)
# 项目名称
project(${projectName})

# include(auto_version.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # 静态库位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # 动态库位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # 可执行文件位置
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin) set(LIBRARY_OUTPUT_PATH
# ${CMAKE_SOURCE_DIR}/bin)

# add_subdirectory(src) add_subdirectory(tests)
add_subdirectory(benchmark)
