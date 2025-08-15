cmake_minimum_required(VERSION 3.24)
project(graphics
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "DX graphics library")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES Graphics.cpp
                 dxerr.cpp)

set(HEADER_FILES dxerr.h
                 DXGetErrorString.inl
                 DXGetErrorDescription.inl
                 DXTrace.inl)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(graphics PRIVATE window)

# Добавляем заголовочные файлы в цель, чтобы они были доступны при сборке(видимы в IDE)
target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

# добавляет текущую директорию в список путей для поиска заголовочных файлов для других проектов/библиотек
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# настройки компиляции
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/Ot>                # Оптимизация по скорости (включена в /O2)
        $<$<CONFIG:Debug>:/Od /fp:fast>)        # Отключение оптимизации (по умолчанию в Debug)


# clang-format настройки
find_program(CLANG_FORMAT_EXE NAMES clang-format clang-format-15 clang-format-14 clang-format-13)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

    add_custom_target(${PROJECT_NAME}_format
            COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${SOURCE_FILES} ${HEADER_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Formatting source files with clang-format"
    )
else()
    message(WARNING "Could not locate clang-format for models - formatting target will not be available")
endif()



