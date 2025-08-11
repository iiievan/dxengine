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

set(SOURCE_FILES prokaryotic.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

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

# clang-format настройки
find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-15 clang-format-14 clang-format-13)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

    add_custom_target(${PROJECT_NAME}_format
            COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files with clang-format > cmake --build . --target format_models"
    )
else()
    message(WARNING "Could not locate clang-format for models - formatting target will not be available")
endif()

add_subdirectory(camera)
add_subdirectory(d3d_api)
add_subdirectory(light)
add_subdirectory(models)
add_subdirectory(shader_api)
add_subdirectory(textures)
add_subdirectory(utils)
add_subdirectory(window)



