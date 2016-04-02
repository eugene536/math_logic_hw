CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
PROJECT(Task)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -DDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

SET(SOURCE_FILES tree.h
                 tree.cpp
                 parser.h
                 parser.cpp
                 kripke.h
                 kripke.cpp
                 main.cpp)


FILE(COPY ${CMAKE_HOME_DIRECTORY}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(Task ${SOURCE_FILES})

