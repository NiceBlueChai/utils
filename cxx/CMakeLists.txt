cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(utils VERSION 1 LANGUAGES CXX)

option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable unit tests: ${ENABLE_UNIT_TESTS}")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

option(BUILD_WITH_QT "Build with Qt" ON)
option(BUILD_WITH_BOOST "Build with Boost " ON)

set(EXT_LIBS "")
if (BUILD_WITH_QT)
    add_definitions(-DUTILS_BUILD_WITH_QT)
    list(APPEND EXT_LIBS Qt5::Core)
endif ()
if(BUILD_WITH_BOOST)
    add_definitions(-DUTILS_BUILD_WITH_BOOST)
    find_package(Boost REQUIRED)
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    list(APPEND EXT_LIBS Boost::headers)
endif()
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB SOURCE_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/src/utils/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/include/utils/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/utils/*.h
)

# 判断平台
if(WIN32)
    file(GLOB PLATFORM_SOURCE_FILES 
        ${CMAKE_CURRENT_LIST_DIR}/src/utils/win/*.cc
        ${CMAKE_CURRENT_LIST_DIR}/include/utils/win/*.h
    )
else()
    file(GLOB PLATFORM_SOURCE_FILE ${CMAKE_CURRENT_LIST_DIR}/src/utils/linux/*.cc  ${CMAKE_CURRENT_LIST_DIR}/include/utils/linux/*.h)
endif()

add_library(utils 
   ${PLATOFRM_SOURCE_FILES}
   ${SOURCE_FILES}
)
target_link_libraries(utils PUBLIC ${EXT_LIBS})
target_include_directories(utils PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/utils)
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
