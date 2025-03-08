set(UPDATER_EXECUTABLE_NAME "updater")

file(GLOB SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${COMMON_DIR}/src/*.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${COMMON_DIR}/include)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

add_executable(${UPDATER_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${UPDATER_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
                           cpr::cpr
)

target_link_libraries(${UPDATER_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
                      cpr::cpr
                      jwt-cpp
)

if(APPLE)
    set_target_properties(${UPDATER_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@loader_path/../lib"
        MACOSX_RPATH 1
    )
else()
    set_target_properties(${UPDATER_EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN/../lib"
    )
endif()

install(TARGETS ${UPDATER_EXECUTABLE_NAME} DESTINATION bin)
