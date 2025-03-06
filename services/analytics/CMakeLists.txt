set(ANALYTICS_EXECUTABLE_NAME "analytics")

file(GLOB SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${COMMON_DIR}/src/*.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${COMMON_DIR}/include)

add_executable(${ANALYTICS_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${ANALYTICS_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${ANALYTICS_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
                      cpr::cpr
)

if(APPLE)
    set_target_properties(${ANALYTICS_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../lib"
        MACOSX_RPATH 1
    )
else()
    set_target_properties(${ANALYTICS_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN/../lib"
    )
endif()

install(TARGETS ${ANALYTICS_EXECUTABLE_NAME} DESTINATION bin)
