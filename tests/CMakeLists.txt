cmake_minimum_required (VERSION 3.5)
project (unit_tests LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


IF (WIN32)
    include_directories (D:/Lib/googletest/googletest/include/)
    link_directories (D:/Lib/googletest/build/lib/)
ENDIF()

# set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/navigation
    )

add_executable(
    test_common
    test_common.cpp
    )

add_executable(
    test_AirArea
    test_AirArea.cpp
    )

set(LIBS libgtest.a Qt5::Core)

target_link_libraries(
    test_common
    ${LIBS}
    )


target_link_libraries(
    test_AirArea
    ${LIBS}
    )

