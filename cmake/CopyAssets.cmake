function(copy_assets target_name)
    message(STATUS "Setting up asset copying for target: ${target_name}")

    # Список папок для копирования
    set(ASSET_FOLDERS models textures)

    foreach(folder IN LISTS ASSET_FOLDERS)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${folder}")
            message(STATUS "${folder} folder found, adding copy command")
            add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target_name}>/${folder}/"
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_CURRENT_SOURCE_DIR}/${folder}/"
                    "$<TARGET_FILE_DIR:${target_name}>/${folder}/"
                    COMMENT "Copying ${folder} to output directory"
            )
        else()
            message(STATUS "${folder} folder not found, skipping")
        endif()
    endforeach()
endfunction()