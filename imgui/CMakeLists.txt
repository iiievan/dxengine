cmake_minimum_required(VERSION 3.24)
project(imgui
        VERSION 1.69
        LANGUAGES CXX
        DESCRIPTION
        "ImGUI library")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES imgui.cpp
                 imgui_demo.cpp
                 imgui_draw.cpp
                 imgui_impl_dx11.cpp
                 imgui_impl_win32.cpp
                 imgui_widgets.cpp)

set(HEADER_FILES imgui.h
                 imconfig.h
                 imgui_impl_dx11.h
                 imgui_impl_win32.h
                 imgui_internal.h
                 imstb_rectpack.h
                 imstb_textedit.h
                 imstb_truetype.h)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                     PRIVATE
                     window
                     graphics)

# Добавляем заголовочные файлы в цель, чтобы они были доступны при сборке(видимы в IDE)
target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

# добавляет текущую директорию в список путей для поиска заголовочных файлов для других проектов/библиотек
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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



