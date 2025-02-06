set(DATA_COLLECTION_EXECUTABLE_NAME "data_collection") 

set(SRC_FILES
    main.cpp
)

set(HEADER_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/"
)

include_directories(${HEADER_DIR})

find_package(Boost REQUIRED) 
find_package(Threads REQUIRED)  

add_executable(${DATA_COLLECTION_EXECUTABLE_NAME} ${SRC_FILES})

target_include_directories(${DATA_COLLECTION_EXECUTABLE_NAME}
                           PRIVATE 
                           ${CROW_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${DATA_COLLECTION_EXECUTABLE_NAME} 
                      ${CROW_LIBRARY} 
                      Threads::Threads 
                      ${Boost_LIBRARIES}
)

install(TARGETS ${DATA_COLLECTION_EXECUTABLE_NAME} DESTINATION bin)
