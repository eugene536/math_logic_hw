cmake_minimum_required(VERSION 2.8.4)
project(Task)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -DDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -g")

SET(SOURCE_FILES tree.h
                 tree.cpp
                 parser.h
                 parser.cpp
                 deduction.h
                 deduction.cpp
                 tests.cpp)

file(INSTALL ${CMAKE_HOME_DIRECTORY}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

cmake_minimum_required (VERSION 2.6)
find_package (Threads)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(Task ${SOURCE_FILES})

target_link_libraries(Task ${GTEST_BOTH_LIBRARIES})
target_link_libraries (Task ${CMAKE_THREAD_LIBS_INIT})

