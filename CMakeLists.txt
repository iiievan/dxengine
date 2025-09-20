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

include_directories(${PROJECT_SOURCE_DIR}/resources)

# Подключаем модуль и настраиваем ресурсы
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ConfigureResources)
configure_application_resources(${PROJECT_NAME})

# Подключаем модуль для копирования DLL Assimp
include(CopyDLL)
add_subdirectory(assimp)  # ← ДОЛЖНО БЫТЬ ПЕРЕД add_executable!

add_executable(${PROJECT_NAME} WIN32 ${RC_FILE} app/WinMain.cpp
                                                app/App.cpp
                                                app/ChiliTimer.cpp)
# Подключаем модуль и компилируем шейдеры
include(CompileHLSL)
compile_hlsl_shaders()

# Добавляем зависимость если шейдеры были скомпилированы
if(HLSL_COMPILED)
    add_dependencies(${PROJECT_NAME} compile_shaders)
endif()

# настройки препроцессора для всего проекта
add_compile_definitions($<$<CONFIG:Release>:NDEBUG>
                        $<$<CONFIG:Debug>:IS_DEBUG=true>
                        $<$<CONFIG:Release>:IS_DEBUG=false>)

# Unicode определения только для Windows
#if(WIN32)
#    add_compile_definitions(UNICODE _UNICODE)
#endif()

include(SetCommonCompileSettings)

# Применяем настройки к основному исполняемому файлу
set_common_compile_settings(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME} 
                      PRIVATE
                      utils
                      graphics
                      window
                      dxbindraw
                      imgui
                      assimp
                      kernel32.lib  # OutputDebugString needs kernel32.lib
                      user32.lib
                      d3d11
                      d3dcompiler
                      gdiplus)

# Копируем DLL assimp в выходную директорию
copy_assimp_dll(${PROJECT_NAME})

add_subdirectory(utils)
add_subdirectory(window)
add_subdirectory(graphics)
add_subdirectory(dxbindraw)
add_subdirectory(imgui)

# После добавления всех поддиректорий применяем настройки ко всем библиотекам
set_common_compile_settings(utils)
set_common_compile_settings(window)
set_common_compile_settings(graphics)
set_common_compile_settings(dxbindraw)
