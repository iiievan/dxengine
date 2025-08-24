cmake_minimum_required(VERSION 3.24)
project(bindable
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "Bindable objects DirectX API")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES Bindable.cpp
                 IndexBuffer.cpp
                 InputLayout.cpp
                 PixelShader.cpp
                 Topology.cpp
                 TransformCbuf.cpp
                 VertexBuffer.cpp
                 VertexShader.cpp)

set(HEADER_FILES Bindable.h
                 BindableBase.h
                 ConstantBuffer.h
                 IndexBuffer.h
                 InputLayout.h
                 PixelShader.h
                 Topology.h
                 TransformCbuf.h
                 VertexBuffer.h
                 VertexShader.h)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      window
                      graphics)

# Добавляем заголовочные файлы в цель, чтобы они были доступны при сборке(видимы в IDE)
target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

# добавляет текущую директорию в список путей для поиска заголовочных файлов для других проектов/библиотек
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# настройки компиляции
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:Debug>:IS_DEBUG=true>
        $<$<CONFIG:Release>:IS_DEBUG=false>)

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



