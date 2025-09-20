# cmake/CopyDLL.cmake
function(copy_dll target_name dll_path)
    message(STATUS "Checking for DLL at: ${dll_path}")
    if(WIN32 AND EXISTS "${dll_path}")
        message(STATUS "DLL found, adding copy command for target: ${target_name}")
        add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${dll_path}"
                "$<TARGET_FILE_DIR:${target_name}>/"
                COMMENT "Copying DLL to output directory: $<TARGET_FILE_DIR:${target_name}>/"
        )
    else()
        message(WARNING "DLL not found at: ${dll_path}")
    endif()
endfunction()

# Функция специально для assimp
function(copy_assimp_dll target_name)
    copy_dll(${target_name} "${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/assimp-vc143-mt.dll")
endfunction()