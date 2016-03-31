CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
PROJECT(Task)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -Wextra -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -DDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

SET(SOURCE_FILES tree.h
                 tree.cpp
                 parser.h
                 parser.cpp
                 deduction.h
                 deduction.cpp)


FILE(COPY ${CMAKE_HOME_DIRECTORY}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

IF (0)
    SET(SOURCE_FILES ${SOURCE_FILES} tests.cpp)

    FIND_PACKAGE(Threads)
    ENABLE_TESTING()
    FIND_PACKAGE(GTest REQUIRED)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

    ADD_EXECUTABLE(Task ${SOURCE_FILES})

    TARGET_LINK_LIBRARIES(Task ${GTEST_BOTH_LIBRARIES})
    TARGET_LINK_LIBRARIES(Task ${CMAKE_THREAD_LIBS_INIT})
ELSE()
    SET(SOURCE_FILES ${SOURCE_FILES} main.cpp)

    ADD_EXECUTABLE(Task ${SOURCE_FILES})
ENDIF()

