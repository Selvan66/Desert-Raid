cmake_minimum_required(VERSION 3.1.0)
project(Game)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Project file
set(CODE_DIR ${CMAKE_SOURCE_DIR}/code)
file(GLOB_RECURSE CODE LIST_DIRECTORIES true ${CODE_DIR}/*.cpp ${CODE_DIR}/*.hpp)  
include_directories(${CODE_DIR})                                                                               

# Target setup
add_executable(${EXECUTABLE_NAME} ${CODE})

# SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics audio network)

if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()