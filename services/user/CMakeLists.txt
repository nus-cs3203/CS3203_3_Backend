set(USER_EXECUTABLE_NAME "user")

file(GLOB SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${COMMON_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/services/management/src/management_api_handler.cpp
    ${CMAKE_SOURCE_DIR}/services/management/src/management_api_strategy.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/services/management/include)
include_directories(${COMMON_DIR}/include)

add_executable(${USER_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${USER_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${USER_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
                      cpr::cpr
                      jwt-cpp
)

if(APPLE)
    set_target_properties(${USER_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../lib"
        MACOSX_RPATH 1
    )
else()
    set_target_properties(${USER_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN/../lib"
    )
endif()

install(TARGETS ${USER_EXECUTABLE_NAME} DESTINATION bin)
