set(DB_UPDATER_REALTIME "db_updater_realtime")

set(SRC_FILES
    src/main.cpp
    src/api_handler.cpp
    ${COMMON_DIR}/src/constants.cpp
    ${COMMON_DIR}/src/database.cpp
    ${COMMON_DIR}/src/utils.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${COMMON_DIR}/include)

find_package(Boost REQUIRED) 
find_package(Threads REQUIRED)  
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

add_executable(${DB_UPDATER_REALTIME} ${SRC_FILES})

target_include_directories(${DB_UPDATER_REALTIME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${DB_UPDATER_REALTIME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
                      cpr::cpr
)

install(TARGETS ${DB_UPDATER_REALTIME} DESTINATION bin)
