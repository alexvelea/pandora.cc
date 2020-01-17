include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")

add_executable(hello-world-example ${CMAKE_CURRENT_LIST_DIR}/bin/main.cpp)
set_property(TARGET hello-world-example PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
target_link_libraries(hello-world-example pandora-core ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

add_library(multiplication SHARED ${CMAKE_CURRENT_LIST_DIR}/src/services/multiplication.cpp)
set_property(TARGET multiplication PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
target_link_libraries(multiplication pandora-service)


add_library(addition SHARED ${CMAKE_CURRENT_LIST_DIR}/src/services/addition.cpp)
set_property(TARGET addition PROPERTY COMPILE_FLAGS ${PANDORA_EXEC_COMPILE_FLAGS})
target_link_libraries(addition pandora-service)