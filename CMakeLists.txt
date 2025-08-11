cmake_minimum_required(VERSION 3.26)
project(DXEngine
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "Simple DirectX11 graphics engine")

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/app)
include_directories(${PROJECT_SOURCE_DIR}/prokariotic)

add_executable(${PROJECT_NAME} WIN32 app/main.cpp)


target_link_libraries(${PROJECT_NAME} 
                      PRIVATE
                      prokariotic)

add_subdirectory(prokariotic)
