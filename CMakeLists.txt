cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(CS3203_BACKEND
        LANGUAGES CXX
        VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(EXECUTABLE_NAME "CS3203_BACKEND")

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(services/data_collection/src)
add_subdirectory(services/db_manager)