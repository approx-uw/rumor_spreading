set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

set(BIN_DIR ${PROJECT_DIR}/bin)
set(TEST_DIR ${PROJECT_DIR}/test)
set(EXAMPLE_DIR ${PROJECT_DIR}/examples)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_COLOR_MAKEFILE ON)

if(${USE_CLANG_AND_DIST})
    SET (CMAKE_CXX_COMPILER  "/usr/lib/distcc/clang++")
    set(USE_CLANG 1)
endif()

#file(GLOB_RECURSE boost_interprocess "boost_interprocess/src/*.cpp")
#add_library(boost_interprocess SHARED ${boost_interprocess})

if(${USE_CLANG})
    if(NOT USE_CLANG_AND_DIST)
        SET (CMAKE_CXX_COMPILER  "clang++")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-constant-out-of-range-compare")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu -stdlib=libc++")
    set(LIBS c++)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -fno-strict-aliasing")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe -g")


foreach(dir ${BOOST_SUBS})
    include_directories("${dir}/include/")
endforeach()

add_subdirectory(${PROJECT_DIR}/paal)
    
include_directories("${PROJECT_DIR}/src/include/")
include_directories (${PROJECT_DIR}/paal/include)

file(GLOB BINS "${PROJECT_DIR}/*.cpp")
message(${BINS})
message(${PROJECT_DIR})

foreach(BIN ${BINS})
    get_filename_component(n ${BIN} NAME_WE)
    add_executable(${n} ${BIN})
    #target_link_libraries (${n} boost_iostreams boost_program_options boost_filesystem boost_system boost_interprocess z bz2 -static-libstdc++)
    target_link_libraries (${n} boost_iostreams boost_program_options boost_filesystem boost_system z bz2 rt -static-libstdc++)
endforeach()





