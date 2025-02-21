set(PAGES_ADMIN_MANAGEMENT_EXECUTABLE_NAME "pages_admin_management")

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

add_executable(${PAGES_ADMIN_MANAGEMENT_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${PAGES_ADMIN_MANAGEMENT_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PAGES_ADMIN_MANAGEMENT_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
                      mongo::mongocxx_shared
                      mongo::bsoncxx_shared
)

install(TARGETS ${PAGES_ADMIN_MANAGEMENT_EXECUTABLE_NAME} DESTINATION bin)
