#include "Utils.hpp"

LPCSTR ConvertWideToANSI(LPCWSTR wideStr)
{
    if (!wideStr)
        return nullptr;

    // Вычисляем размер буфера для ANSI-строки
    int sizeNeeded = WideCharToMultiByte(CP_ACP,  // Кодовая страница (ANSI)
                                         0,       // Флаги
                                         wideStr, // Исходная строка
                                         -1,      // Длина (автоматически, если -1)
                                         nullptr, // Выходной буфер (nullptr для расчёта размера)
                                         0,       // Размер буфера (0 при nullptr)
                                         nullptr,
                                         nullptr // Доп. параметры
    );

    if (sizeNeeded <= 0)
        return nullptr;

    // Выделяем буфер
    char *ansiStr = new char[sizeNeeded];
    WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ansiStr, sizeNeeded, nullptr, nullptr);

    return ansiStr;
}
