cmake_minimum_required(VERSION 3.26)
project(prokariotic
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "DirectX11 graphics engine")

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/camera
                    ${PROJECT_SOURCE_DIR}/d3d_api
                    ${PROJECT_SOURCE_DIR}/light
                    ${PROJECT_SOURCE_DIR}/models
                    ${PROJECT_SOURCE_DIR}/shader_api
                    ${PROJECT_SOURCE_DIR}/textures
                    ${PROJECT_SOURCE_DIR}/utils
                    ${PROJECT_SOURCE_DIR}/window)

add_library(${PROJECT_NAME} STATIC prokaryotic.cpp
        light/LightClass.hpp
        light/LightClass.cpp)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        camera
        d3d_api
        light
        models
        shader_api
        textures
        utils
        window
        d3d11
        d3dcompiler
)

add_subdirectory(camera)
add_subdirectory(d3d_api)
add_subdirectory(light)
add_subdirectory(models)
add_subdirectory(shader_api)
add_subdirectory(textures)
add_subdirectory(utils)
add_subdirectory(window)



