cmake_minimum_required(VERSION 3.26)
project(DXEngine
        VERSION 1.0
        LANGUAGES CXX RC
        DESCRIPTION
        "Simple DirectX11 graphics engine")

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include_directories(${PROJECT_SOURCE_DIR}/app
                    ${PROJECT_SOURCE_DIR}/resources
                    ${PROJECT_SOURCE_DIR}/utils
                    ${PROJECT_SOURCE_DIR}/graphics
                    ${PROJECT_SOURCE_DIR}/window)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ConfigureResources)

# Настраиваем ресурсы
configure_application_resources(${PROJECT_NAME})


add_executable(${PROJECT_NAME} WIN32 ${RC_FILE} app/WinMain.cpp
                                                app/App.cpp
                                                app/ChiliTimer.cpp)


target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/Ot>                # Оптимизация по скорости (включена в /O2)
        $<$<CONFIG:Debug>:/Od /fp:fast>         # Отключение оптимизации (по умолчанию в Debug)
)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:Debug>:/MTd>
)

target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:Debug>:/MTd>
)

target_link_libraries(${PROJECT_NAME} 
                      PRIVATE
                      utils
                      graphics
                      window
                      kernel32.lib  # OutputDebugString needs kernel32.lib
                      user32.lib
                      d3d11
                      d3dcompiler)

add_subdirectory(utils)
add_subdirectory(window)
add_subdirectory(graphics)


