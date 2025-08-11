cmake_minimum_required(VERSION 3.24)
project(models
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "Models api")
set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC ModelClass.cpp)


# clang-format
find_program(clang_format NAMES clang-format)
if (NOT clang_format)
    message(STATUS "Could not locate clang-format")
else ()
    file(GLOB format_files ${FORMATTING})
    add_custom_target(${PROJECT_NAME}_format_code COMMAND ${clang_format} -i -fallback-style=none -style=file --verbose ${format_files})
endif ()



