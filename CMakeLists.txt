cmake_minimum_required(VERSION 3.1.0)
project(Game)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Project file
set(CODE_DIR ${CMAKE_SOURCE_DIR}/code)
file(GLOB_RECURSE CODE LIST_DIRECTORIES true ${CODE_DIR}/*.cpp ${CODE_DIR}/*.h)                                                                                   

set(SFML_STATIC_LIBRARIES TRUE)
add_subdirectory(SFML)

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
file(COPY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)