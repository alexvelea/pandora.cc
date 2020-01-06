SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pandora)

cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_DIR      "src/")
set(INCLUDE_DIR     "include/")

set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

include_directories(${INCLUDE_DIR})

set(PANDORACC_SRCS
        src/services.cpp)

add_library(pandora ${PANDORACC_SRCS} )

find_package (Threads)
include(examples/all.cmake)