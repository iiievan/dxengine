# Функция для применения стандартных настроек компиляции
function(set_common_compile_settings target)
    target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/Ot>                # Оптимизация по скорости
            $<$<CONFIG:Debug>:/Od>                  # Отключение оптимизации
            $<$<CONFIG:Debug>:/fp:fast>             # Быстрая математика только для Debug
    )

    target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/MT>
            $<$<CONFIG:Debug>:/MTd>
    )

    target_link_options(${target} PRIVATE
            $<$<CONFIG:Release>:/MT>
            $<$<CONFIG:Debug>:/MTd>
    )
endfunction()