
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
add_executable(hello-world-example
        ${CMAKE_CURRENT_LIST_DIR}/bin/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/services/addition.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/services/multiplication.cpp)

target_link_libraries(hello-world-example pandora ${CMAKE_THREAD_LIBS_INIT})