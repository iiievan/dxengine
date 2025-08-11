cmake_minimum_required(VERSION 3.24)
project(d3d_api
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "Direct3D graphics api")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES D3DClass.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# clang-format настройки
find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-15 clang-format-14 clang-format-13)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

    add_custom_target(${PROJECT_NAME}_format
            COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files with clang-format"
    )
else()
    message(WARNING "Could not locate clang-format for models - formatting target will not be available")
endif()



