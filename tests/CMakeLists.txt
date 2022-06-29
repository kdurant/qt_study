cmake_minimum_required (VERSION 3.5)
project (unit_tests LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

IF (WIN32)
    include_directories (D:/Lib/googletest/googletest/include/)
    link_directories (D:/Lib/googletest/build/lib/)
ENDIF()

find_package(Qt5 COMPONENTS Core REQUIRED)

include_directories(
    ..
    )

add_executable(
    ${PROJECT_NAME}
    test_common.cpp
    )

target_link_libraries(
    ${PROJECT_NAME}
    libgtest.a
    Qt5::Core 
  )

