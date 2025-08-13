function(configure_application_resources TARGET_NAME)
    if(WIN32)
        # Путь к папке с иконками
        set(ICONS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

        # Создаем директорию ресурсов в билде
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated_resources")

        # Ищем все .ico файлы в папке resources
        file(GLOB ICON_FILES
                LIST_DIRECTORIES false
                RELATIVE "${ICONS_DIR}"
                "${ICONS_DIR}/*.ico"
        )

        if(NOT ICON_FILES)
            message(FATAL_ERROR "No .ico files found in: ${ICONS_DIR}")
        endif()

        # Начинаем формировать RC-файл
        set(RC_CONTENT "#include \"resources.h\"\n\n")

        # Обрабатываем каждую найденную иконку
        set(ICON_ID 101)  # Начальный ID
        set(RESOURCES_H_CONTENT "// Auto-generated resources IDs\n")
        foreach(ICON_FILE ${ICON_FILES})
            # Копируем иконку в билд-директорию
            file(COPY "${ICONS_DIR}/${ICON_FILE}"
                    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/generated_resources")

            # Генерируем уникальное имя для ID
            get_filename_component(ICON_BASENAME "${ICON_FILE}" NAME_WE)
            string(MAKE_C_IDENTIFIER "${ICON_BASENAME}" ICON_VAR_NAME)
            string(TOUPPER "${ICON_VAR_NAME}" ICON_VAR_NAME)
            set(CURRENT_ID "IDI_${ICON_VAR_NAME}_ICON")

            # Добавляем запись в RC-файл
            string(APPEND RC_CONTENT
                    "${CURRENT_ID} ICON \"${CMAKE_CURRENT_BINARY_DIR}/generated_resources/${ICON_FILE}\"\n")

            # Добавляем определение в resources.h
            string(APPEND RESOURCES_H_CONTENT "#define ${CURRENT_ID} ${ICON_ID}\n")

            math(EXPR ICON_ID "${ICON_ID} + 1")  # Увеличиваем ID
        endforeach()

        # Записываем RC-файл
        set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated_resources/app_resources.rc")
        file(WRITE "${RC_FILE}" "${RC_CONTENT}")

        # Добавляем блок для Visual Studio Resource Editor
        string(APPEND RESOURCES_H_CONTENT "\n// Visual Studio Resource Editor support\n")
        string(APPEND RESOURCES_H_CONTENT "#ifdef APSTUDIO_INVOKED\n")
        string(APPEND RESOURCES_H_CONTENT "#ifndef APSTUDIO_READONLY_SYMBOLS\n")
        string(APPEND RESOURCES_H_CONTENT "#define _APS_NEXT_RESOURCE_VALUE    ${ICON_ID}\n")
        string(APPEND RESOURCES_H_CONTENT "#define _APS_NEXT_COMMAND_VALUE    40001\n")
        string(APPEND RESOURCES_H_CONTENT "#define _APS_NEXT_CONTROL_VALUE    1001\n")
        string(APPEND RESOURCES_H_CONTENT "#define _APS_NEXT_SYMED_VALUE      101\n")
        string(APPEND RESOURCES_H_CONTENT "#endif\n")
        string(APPEND RESOURCES_H_CONTENT "#endif\n")

        # Генерируем/обновляем resources.h в исходной директории
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.h"
                "#ifndef RESOURCES_H\n#define RESOURCES_H\n\n${RESOURCES_H_CONTENT}\n#endif // RESOURCES_H\n")

        # Возвращаем путь к RC-файлу
        set(RC_FILE "${RC_FILE}" PARENT_SCOPE)

        message(STATUS "Generated resources.h with ${ICON_ID} icon IDs")
    endif()
endfunction()