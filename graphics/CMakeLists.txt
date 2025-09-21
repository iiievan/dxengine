cmake_minimum_required(VERSION 3.24)
project(graphics
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "DX graphics library")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES Graphics.cpp
                 dxerr.cpp
                 DxgiInfoManager.cpp
                 Surface.cpp
                 GDIPlusManager.cpp
                 ImguiManager.cpp
                 Camera.cpp
                 PointLight.cpp)

set(HEADER_FILES GDIPlusManager.h
                 DxgiInfoManager.h
                 Surface.h
                 ImguiManager.h
                 GraphicsThrowMacroses.h
                 PointLight.h
                 Camera.h
                 dxerr.h
                 DXGetErrorString.inl
                 DXGetErrorDescription.inl
                 DXTrace.inl
                 VertexLayout.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      window
                      dxbindraw
                      imgui
                      utils)

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



