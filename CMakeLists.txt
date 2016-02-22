set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe -g")

find_package( Boost REQUIRED COMPONENTS program_options system)
include_directories( ${Boost_INCLUDE_DIRS} )

file(GLOB BIN_SRCS "${PROJECT_DIR}/*.cpp")

foreach(BIN_SRC ${BIN_SRCS})
    get_filename_component(BIN_NAME ${BIN_SRC} NAME_WE)
    add_executable(${BIN_NAME} ${BIN_SRC})
    target_link_libraries (${BIN_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}  z bz2 rt -static-libstdc++)
endforeach()

