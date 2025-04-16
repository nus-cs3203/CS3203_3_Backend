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

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_MACOSX_RPATH 1)

set(EXECUTABLE_NAME "CS3203_BACKEND")

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.11.2  
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.7.0
)
FetchContent_MakeAvailable(jwt-cpp)

include(CTest)
enable_testing()
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(services/analytics)
add_subdirectory(services/management)
add_subdirectory(services/updater)
add_subdirectory(services/user)

option(ENABLE_TESTING "Build tests" ON)
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()