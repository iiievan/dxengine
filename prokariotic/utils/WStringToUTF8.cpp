#include "Utils.hpp"

// Преобразует std::wstring в std::string (UTF-8)
std::string WStringToUTF8(const std::wstring &wstr)
{
    if (wstr.empty())
        return {};

    // Узнаем нужный размер буфера
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);

    if (size_needed <= 0)
        return {}; // Ошибка конвертации

    // Создаем строку с нужным размером
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), result.data(), size_needed, nullptr, nullptr);

    return result;
}
