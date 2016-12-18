cmake_minimum_required(VERSION 2.8.0)
project(umonibench_jaguar_pinger)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release")
endif ()


if (NOT DEP_PATH)
    SET(DEP_PATH ${PROJECT_SOURCE_DIR}/deps/compiled/ubuntu_64)
endif ()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -ggdb -pg ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX -ggdb  -pg")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${DEP_PATH}/lib")


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


find_path(Muduo_INCLUDE_DIR muduo "${DEP_PATH}/include")
find_path(Muduo_LIBRARY_DIR libmuduo_net.a "${DEP_PATH}/lib")
message(STATUS ${Muduo_INCLUDE_DIR})
message(STATUS ${Muduo_LIBRARY_DIR})
include_directories(${Muduo_INCLUDE_DIR})
find_library(muduo_base muduo_base)
find_library(muduo_net muduo_net)
message(STATUS ${muduo_base})
message(STATUS ${muduo_net})


find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})



add_subdirectory(jaguar/common)
add_subdirectory(jaguar/core)
add_subdirectory(jaguar/pinger)
add_subdirectory(jaguar/mock)

