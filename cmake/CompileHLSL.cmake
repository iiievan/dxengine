# cmake/CompileHLSL.cmake
function(compile_hlsl_shaders)
    # Находим компилятор HLSL
    find_program(FXC_COMPILER fxc.exe
            PATHS
            "$ENV{VulkanSDK}/Bin"
            "$ENV{DXSDK_DIR}Utilities/Bin/x86"
            "C:/Program Files (x86)/Windows Kits/10/bin/**"
            "C:/Program Files (x86)/Windows Kits/8.1/bin/x86"
            "C:/Program Files (x86)/Windows Kits/8.0/bin/x86"
    )

    if(NOT FXC_COMPILER)
        message(WARNING "HLSL compiler (fxc.exe) not found. Shaders will not be compiled.")
        return()
    endif()

    message(STATUS "Found HLSL compiler: ${FXC_COMPILER}")

    # Ищем все HLSL файлы в указанных директориях
    set(SHADER_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders"
    )

    set(HLSL_FILES)
    foreach(DIR ${SHADER_DIRS})
        if(EXISTS ${DIR})
            file(GLOB_RECURSE FOUND_FILES "${DIR}/*.hlsl")
            list(APPEND HLSL_FILES ${FOUND_FILES})
        endif()
    endforeach()

    if(NOT HLSL_FILES)
        message(STATUS "No HLSL shader files found")
        return()
    endif()

    set(CSO_FILES)

    foreach(HLSL_FILE ${HLSL_FILES})
        get_filename_component(FILE_NAME ${HLSL_FILE} NAME_WE)
        get_filename_component(FILE_DIR ${HLSL_FILE} DIRECTORY)

        # Определяем тип шейдера по имени файла или расширению
        if(HLSL_FILE MATCHES ".*\\.vs\\.hlsl$")
            set(SHADER_MODEL "vs_5_0")
            set(ENTRY_POINT "VSMain")
            set(FILE_SUFFIX ".vs")
        elseif(HLSL_FILE MATCHES ".*\\.ps\\.hlsl$")
            set(SHADER_MODEL "ps_5_0")
            set(ENTRY_POINT "PSMain")
            set(FILE_SUFFIX ".ps")
        elseif(HLSL_FILE MATCHES ".*\\.cs\\.hlsl$")
            set(SHADER_MODEL "cs_5_0")
            set(ENTRY_POINT "CSMain")
            set(FILE_SUFFIX ".cs")
        elseif(HLSL_FILE MATCHES ".*\\.gs\\.hlsl$")
            set(SHADER_MODEL "gs_5_0")
            set(ENTRY_POINT "GSMain")
            set(FILE_SUFFIX ".gs")
        elseif(FILE_NAME MATCHES ".*[Vv]ertex.*")
            set(SHADER_MODEL "vs_5_0")
            set(ENTRY_POINT "VSMain")
            set(FILE_SUFFIX ".vs")
        elseif(FILE_NAME MATCHES ".*[Pp]ixel.*")
            set(SHADER_MODEL "ps_5_0")
            set(ENTRY_POINT "PSMain")
            set(FILE_SUFFIX ".ps")
        else()
            set(SHADER_MODEL "vs_5_0")
            set(ENTRY_POINT "main")
            set(FILE_SUFFIX "")
        endif()

        # Правильное получение относительного пути
        file(RELATIVE_PATH RELATIVE_PATH
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${FILE_DIR}"
        )

        # Если файл в корневой директории проекта
        if(RELATIVE_PATH STREQUAL "")
            set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
            # Если файл в папке shaders/ (самый частый случай)
        elseif(RELATIVE_PATH STREQUAL "shaders")
            set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
            # Если файл в подпапке shaders/effects/ и т.д.
        else()
            string(REPLACE "shaders/" "" RELATIVE_SUBPATH ${RELATIVE_PATH})
            set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/${RELATIVE_SUBPATH}")
        endif()

        # Создаем имя файла с суффиксом типа шейдера
        set(OUTPUT_FILE "${OUTPUT_DIR}/${FILE_NAME}${FILE_SUFFIX}.cso")

        # Создаем директорию для выходного файла
        file(MAKE_DIRECTORY ${OUTPUT_DIR})

        add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND ${FXC_COMPILER}
                /nologo
                /T ${SHADER_MODEL}
                /E ${ENTRY_POINT}
                /Zi  # ←  генерация отладочной информации
                /Od  # ←  отключение оптимизаций
                /Fo "${OUTPUT_FILE}"
                "${HLSL_FILE}"
                DEPENDS ${HLSL_FILE}
                COMMENT "Compiling HLSL shader: ${FILE_NAME}.hlsl -> ${FILE_NAME}${FILE_SUFFIX}.cso"
                VERBATIM
        )

        list(APPEND CSO_FILES ${OUTPUT_FILE})
        source_group("shaders" FILES ${HLSL_FILE})

    endforeach()

    # Создаем цель для компиляции всех шейдеров
    add_custom_target(compile_shaders ALL DEPENDS ${CSO_FILES})

    # Копируем скомпилированные шейдеры в выходную директорию приложения
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_BINARY_DIR}/shaders"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
            COMMENT "Copying compiled shaders to output directory"
    )

    # Добавляем директорию с шейдерами в include для IDE
    include_directories(${SHADER_DIRS})

    message(STATUS "Found ${HLSL_FILES} HLSL shader files for compilation")

    set(HLSL_COMPILED TRUE CACHE INTERNAL "HLSL shaders compiled")

endfunction()