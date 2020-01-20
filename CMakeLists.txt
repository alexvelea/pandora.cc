SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pandora)

cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR      "src/")
set(INCLUDE_DIR     "include/")

#set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/lib")
#set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

set(LIB_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(BIN_PATH                                   "${CMAKE_CURRENT_SOURCE_DIR}/build")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

add_subdirectory(third-party/autojson)
target_compile_options(autojson PUBLIC -fPIC)
add_subdirectory(third-party/easyflags)
target_compile_options(easyflags PUBLIC -fPIC)



add_library(pandora-service src/bundle.cpp)
set_property(TARGET pandora-service PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
target_include_directories(pandora-service PUBLIC ${INCLUDE_DIR})
target_compile_options(pandora-service PUBLIC -Wall -ansi -pedantic -fPIC -rdynamic)
target_link_libraries(pandora-service autojson easyflags)

add_library(pandora-core src/core.cpp src/bundle.cpp)
set_property(TARGET pandora-core PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
target_include_directories(pandora-core PUBLIC ${INCLUDE_DIR})
target_compile_options(pandora-core PUBLIC -Wall -ansi -pedantic -fPIC -rdynamic)
target_link_libraries(pandora-core autojson easyflags)

add_library(loadService SHARED src/services/loadService.cpp)
target_link_libraries(loadService pandora-service)

add_library(networkHandler SHARED src/services/networkHandler.cpp)
target_link_libraries(networkHandler pandora-service)

add_library(ls SHARED src/services/ls.cpp)
target_link_libraries(ls pandora-service)

find_package (Threads)
add_subdirectory(examples/)