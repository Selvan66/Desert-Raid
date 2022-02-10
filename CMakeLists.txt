cmake_minimum_required(VERSION 3.1.0)
project(Game)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Defines
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Project file
set(CODE_DIR ${CMAKE_SOURCE_DIR}/code)
file(GLOB_RECURSE CODE LIST_DIRECTORIES true ${CODE_DIR}/*.cpp ${CODE_DIR}/*.hpp)  
include_directories(${CODE_DIR})                                                                               

set(SFML_STATIC_LIBRARIES TRUE)
add_subdirectory(SFML)

# Target setup
add_executable(${EXECUTABLE_NAME} ${CODE})

# SFML
include_directories(${CMAKE_SOURCE_DIR}/SFML/include)
target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-system
    sfml-window
    sfml-audio
    sfml-network)

# Copying assets
set(RES_DIR ${CMAKE_SOURCE_DIR}/assets)
file(COPY ${RES_DIR} DESTINATION ${CMAKE_SOURCE_DIR}/bin)