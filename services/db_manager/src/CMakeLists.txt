set(DB_MANAGER_EXECUTABLE_NAME "db_manager")

set(SRC_FILES
    main.cpp
)

set(HEADER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/"
)

include_directories(${HEADER_DIR})

find_package(Boost REQUIRED) 
find_package(Threads REQUIRED)  

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
)

install(TARGETS ${DB_MANAGER_EXECUTABLE_NAME} DESTINATION bin)
