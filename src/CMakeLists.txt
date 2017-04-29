cmake_minimum_required (VERSION 2.8.12.2)
project(graphisoft_competition_2017)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

set (CXX_COMMON_FLAGS "-std=c++14 -Wall -Wextra")
set (CMAKE_CXX_FLAGS_DEBUG "${CXX_COMMON_FLAGS} -O0 -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CXX_COMMON_FLAGS} -O2")

find_package(Boost COMPONENTS program_options system unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory (cpp-util)
add_subdirectory (tools)
add_subdirectory (semifinal)
