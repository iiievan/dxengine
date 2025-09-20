cmake_minimum_required(VERSION 3.26)
project(assimp
        VERSION 6.0.2   # tag 6.0.2 commit fb375dd8c0a032106a2122815fb18dffe0283721 (8.06.2025)
        LANGUAGES CXX
        DESCRIPTION
        "Open Asset Import Library")
set(CMAKE_CXX_STANDARD 17)

# Создаем интерфейсную библиотеку для assimp
add_library(${PROJECT_NAME} INTERFACE)

# Добавляем include директории
target_include_directories(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Добавляем импорт библиотеки и определения
if(WIN32)
    # Добавляем .lib файл для линковки
    target_link_libraries(${PROJECT_NAME} INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc143-mt.lib
    )

    target_compile_definitions(${PROJECT_NAME} INTERFACE
            ASSIMP_DLL
            _CRT_SECURE_NO_WARNINGS
    )
endif()
