set(DB_MANAGER_EXECUTABLE_NAME "db_manager")

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/mongo-cxx-driver" ${CMAKE_PREFIX_PATH})

set(SRC_FILES
    src/main.cpp
    src/api_handler.cpp
    src/database.cpp
    src/utils.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Boost REQUIRED) 
find_package(Threads REQUIRED)  
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

add_executable(${DB_MANAGER_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${DB_MANAGER_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${DB_MANAGER_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
)

install(TARGETS ${DB_MANAGER_EXECUTABLE_NAME} DESTINATION bin)
