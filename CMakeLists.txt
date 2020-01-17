SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pandora)

cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_DIR      "src/")
set(INCLUDE_DIR     "include/")

#set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
#set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

set(PANDORA_EXEC_COMPILE_FLAGS    "-Wall -ansi -pedantic -fPIC -rdynamic")

include_directories(${INCLUDE_DIR})

add_library(pandora-core src/core.cpp src/bundle.cpp)
add_library(pandora-service src/bundle.cpp)

set_property(TARGET pandora-core PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
set_property(TARGET pandora-service PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})

find_package (Threads)
include(examples/all.cmake)