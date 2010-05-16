project(smartalloc)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(DefineCompilerFlags)
include(MacroAddUnitTest)

set(APPLICATION_NAME ${PROJECT_NAME})
set(APPLICATION_VERSION_MAJOR "3")
set(APPLICATION_VERSION_MINOR "1")
set(APPLICATION_VERSION_PATCH "1")
set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")
set(LIBRARY_VERSION "1.0.0")
set(LIBRARY_SOVERSION "1")

add_definitions(-DSMARTALLOC)
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
