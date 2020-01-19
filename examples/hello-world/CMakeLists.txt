include_directories(src)

add_executable(hello-world-example bin/main.cpp)
target_link_libraries(hello-world-example pandora-core ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

add_library(multiplication SHARED src/services/multiplication.cpp)
target_link_libraries(multiplication pandora-service)


add_library(addition SHARED src/services/addition.cpp)
target_link_libraries(addition pandora-service)