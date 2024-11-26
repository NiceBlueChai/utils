cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(utils VERSION 1 LANGUAGES CXX)

option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable unit tests: ${ENABLE_UNIT_TESTS}")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_WITH_QT "Build with Qt" OFF)

if (BUILD_WITH_QT)
    add_definitions(-DUTILS_BUILD_WITH_QT)
endif ()
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# 判断平台

add_library(utils OBJECT
    ${CMAKE_CURRENT_LIST_DIR}/src/utils.cc
    $<$<BOOL:${BUILD_WITH_QT}>:${CMAKE_CURRENT_LIST_DIR}/src/utils-qt.cc>
    $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_LIST_DIR}/src/utils-win.cc>
    $<$<PLATFORM_ID:Linux>:src/utils-linux.cc>
)
if(MSVC)
  target_compile_options(utils PRIVATE /utf-8)
endif()

find_package(GTest REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)
if(ENABLE_UNIT_TESTS)
  enable_testing()
  # allow to report to a cdash dashboard
  include(CTest)
  add_subdirectory(tests)
endif(ENABLE_UNIT_TESTS)
