file(GLOB TEST_SOURCES 
    ${COMMON_DIR}/src/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
    ${CMAKE_SOURCE_DIR}/services/management/src/management_api_handler.cpp
    ${CMAKE_SOURCE_DIR}/services/management/src/management_api_strategy.cpp
    ${CMAKE_SOURCE_DIR}/services/management/src/management_server.cpp
)

add_executable(runTests ${TEST_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${COMMON_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/services/analytics/include)
include_directories(${CMAKE_SOURCE_DIR}/services/management/include)
include_directories(${CMAKE_SOURCE_DIR}/services/user/include)

target_include_directories(runTests
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(runTests
    gtest_main
    Threads::Threads
    ${CROW_LIBRARY} 
    Threads::Threads 
    ${Boost_LIBRARIES}
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
    cpr::cpr
    jwt-cpp
    FakeIt::FakeIt-gtest
)

include(GoogleTest)
gtest_discover_tests(runTests)