cmake_minimum_required(VERSION 3.24)
project(dxbindraw
        VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION
        "DirecX bindable and drawable objects API")
set(CMAKE_CXX_STANDARD 17)

set(BINDABLE_SOURCES
        bindable/Bindable.cpp
        bindable/IndexBuffer.cpp
        bindable/InputLayout.cpp
        bindable/PixelShader.cpp
        bindable/Topology.cpp
        bindable/TransformCbuf.cpp
        bindable/VertexBuffer.cpp
        bindable/VertexShader.cpp
)

set(BINDABLE_HEADERS
        bindable/Bindable.h
        bindable/BindableBase.h
        bindable/ConstantBuffer.h
        bindable/IndexBuffer.h
        bindable/InputLayout.h
        bindable/PixelShader.h
        bindable/Topology.h
        bindable/TransformCbuf.h
        bindable/VertexBuffer.h
        bindable/VertexShader.h
)

set(DRAWABLE_SOURCES
        drawable/Drawable.cpp
        drawable/Box.cpp
)

set(DRAWABLE_HEADERS
        drawable/Drawable.h
        drawable/DrawableBase.h
        drawable/Box.h
)

set(SOURCE_FILES
        ${BINDABLE_SOURCES}
        ${DRAWABLE_SOURCES}
        geometry/Cone.h
        geometry/IndexedTriangleList.h
        geometry/Cube.h
        geometry/Plane.h
        geometry/Prism.h
        geometry/Sphere.h
        drawable/Pyramid.cpp
        drawable/Pyramid.h
        bindable/Texture.cpp
        bindable/Texture.h
        bindable/Sampler.cpp
        bindable/Sampler.h
        drawable/Sheet.cpp
        drawable/Sheet.h
        drawable/SkinnedBox.cpp
        drawable/SkinnedBox.h
        drawable/SolidSphere.cpp
        drawable/SolidSphere.h
        drawable/TestObject.h
        drawable/Cylinder.cpp
        drawable/Cylinder.h
        drawable/AssTest.cpp
        drawable/AssTest.h
)

set(HEADER_FILES
        ${BINDABLE_HEADERS}
        ${DRAWABLE_HEADERS}
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                     PRIVATE
                     imgui
                     utils
                     graphics
                     assimp
                     window)

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



