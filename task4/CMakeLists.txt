cmake_minimum_required(VERSION 2.8.4)
project(Task4)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -DDEBUG")
# -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -g
SET(SOURCE_FILES tree.h
                 tree.cpp
                 main.cpp)

add_executable(Task4 ${SOURCE_FILES})
