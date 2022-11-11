cmake_minimum_required(VERSION 3.10)
set(projectName benchmark)
project(${projectName})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/navigation)

find_package(benchmark REQUIRED)

find_package(
  Qt5
  COMPONENTS Core
  REQUIRED)

find_package(
  Qt5
  COMPONENTS Gui
  REQUIRED)
find_package(
  Qt5
  COMPONENTS Widgets
  REQUIRED)

# add_executable( ${projectName} benchmark.cpp
# ${CMAKE_SOURCE_DIR}/src/navigation/AirArea.cpp
# ${CMAKE_SOURCE_DIR}/src/navigation/CoordCompute.cpp)
# target_link_libraries(${projectName} Qt5::Core Qt5::Widgets Qt5::Gui
# benchmark::benchmark)

# add_executable(optimization optimization.cpp)
# target_link_libraries(optimization Qt5::Core benchmark::benchmark)

add_executable(bench_drawLineWithAngle bench_drawLineWithAngle.cpp)
target_link_libraries(bench_drawLineWithAngle Qt5::Core Qt5::Widgets Qt5::Gui
                      benchmark::benchmark)
