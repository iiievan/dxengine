#ifndef __UTILS_H_
#define __UTILS_H_

#include <Windows.h>
#include <directxmath.h>
#include <filesystem>
#include <string>
#include <unordered_map>


using namespace DirectX;

#define ReleaseCOM(x)     \
    {                     \
        if (x)            \
        {                 \
            x->Release(); \
            x = 0;        \
        }                 \
    }

namespace fs = std::filesystem;

LPCSTR       ConvertWideToANSI(LPCWSTR wideStr);
std::wstring findFullPath(const std::string &targetFileName, bool searchInHidden = false);
std::string  WStringToUTF8(const std::wstring &wstr);

typedef enum
{
    DBG_WHITE = 0,
    DBG_SCARLET,
    DBG_PINK,
    DBG_PURPLE,
    DBG_LIGHT_BLUE,
    DBG_BLUE,
    DBG_AZURE,
    DBG_GREEN,
    DBG_DARK_GREEN,
    DBG_LEMON,
    DBG_AMBER,
    DBG_ORANGE,
    DBG_COLORS_MAX
} DEBUG_COLORS;

class ColorPalette
{
public:
    ColorPalette();
    ~ColorPalette() {}

    XMFLOAT4 GetColor(DEBUG_COLORS color);

private:
    const XMFLOAT4 m_Palette[DBG_COLORS_MAX];
};

class WindowsMessageMap
{
public:
    WindowsMessageMap();
    std::string operator()( DWORD msg,LPARAM lp,WPARAM wp ) const;
private:
    std::unordered_map<DWORD,std::string> map;
};

void DebugPrint(const char* message);

extern ColorPalette color_palette;

#endif // __UTILS_H_
