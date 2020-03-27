cmake_minimum_required(VERSION 3.10)

# set the project name
project(ecunit VERSION 0.0.0)
enable_testing()

set(CMAKE_FLAGS_DEBUG "-Werror -O0 -g3")
set(CMAKE_FLAGS_RELEASE "-Werror -O2")
set(CMAKE_C_FLAGS_DEBUG "-std=c11 ${CMAKE_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "-std=c11 ${CMAKE_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 ${CMAKE_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 ${CMAKE_FLAGS_RELEASE}")

add_subdirectory(src)
add_subdirectory(test)

add_library(ecunit ${SRC_SOURCES})
target_include_directories(ecunit PUBLIC src)
target_include_directories(ecunit PUBLIC include)

add_executable(ecunit-test ${TEST_SOURCES})
target_link_libraries(ecunit-test ecunit)
target_include_directories(ecunit-test PUBLIC include)
add_test(
    NAME ecunit-test
    COMMAND $<TARGET_FILE:ecunit-test>
)
