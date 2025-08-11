#include <algorithm>
#include <iostream>
#include <vector>
#include "Utils.hpp"

// Маркеры корня проекта (можно добавлять свои)
const std::vector<std::string> PROJECT_ROOT_MARKERS = {".git", "README.md"};

// Папки, которые нужно игнорировать
const std::vector<std::string> IGNORE_DIRS =
    {"build", "debug", "release", "bin", "obj", ".vs", "node_modules", ".idea"};

// Проверка, является ли директория корнем проекта
bool isProjectRoot(const fs::path &dir)
{
    for (const auto &marker : PROJECT_ROOT_MARKERS)
    {
        if (fs::exists(dir / marker))
            return true;
    }
    return false;
}

// Поиск корня проекта (поднимаемся вверх)
fs::path findProjectRoot()
{
    fs::path currentDir = fs::current_path(); // Стартуем от текущей папки .exe

    while (currentDir.has_parent_path())
    {
        if (isProjectRoot(currentDir))
            return currentDir;

        currentDir = currentDir.parent_path();
    }

    std::cerr << "Корень проекта не найден! Используется текущая директория.\n";
    return fs::current_path(); // Возвращаем текущую, если корень не определился
}

// Проверка, нужно ли игнорировать папку
bool shouldIgnoreDirectory(const fs::path &dir)
{
    std::string dirName = dir.filename().string();

    return std::find(IGNORE_DIRS.begin(), IGNORE_DIRS.end(), dirName) != IGNORE_DIRS.end();
}

// Рекурсивный поиск файла с выводом всех совпадений
std::wstring findFile(const fs::path &directory, const std::string &targetFileName, bool searchInHidden = false)
{
    try
    {
        for (const auto &entry : fs::directory_iterator(directory))
        {
            // Пропускаем скрытые папки (если не разрешено иное)
            if (!searchInHidden && entry.path().filename().string()._Starts_with("."))
                continue;

            if (entry.is_directory())
            {
                if (!shouldIgnoreDirectory(entry.path()))
                {
                    std::wstring foundPath = findFile(entry.path(), targetFileName, searchInHidden);
                    if (!foundPath.empty()) // Если файл найден во вложенной папке - возвращаем его
                        return foundPath;
                }
            }
            else if (entry.is_regular_file() && entry.path().filename() == targetFileName)
            {
                return entry.path().wstring(); // Возвращаем путь, если файл найден
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "Ошибка доступа к " << directory << ": " << e.what() << std::endl;
    }

    return L"";
}

// Рекурсивный поиск файла с выводом всех совпадений
std::wstring findFullPath(const std::string &targetFileName, bool searchInHidden)
{
    fs::path root_dir = findProjectRoot();

    return findFile(root_dir, targetFileName, searchInHidden);
}
