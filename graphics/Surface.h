#ifndef __SURFACE_H
#define __SURFACE_H

#include "WinDefs.h"
#include "ChiliException.h"
#include <string>
#include <memory>
#include <assert.h>

class Surface
{
public:
    class Color
    {
    public:
        unsigned int dword;
    public:
        constexpr Color() noexcept : dword() { }
        constexpr Color(const Color &col) noexcept : dword(col.dword) { }
        constexpr Color(unsigned int dw) noexcept : dword(dw) { }
        constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
        : dword((x << 24u) | (r << 16u) | (g << 8u) | b)
        { }
        constexpr Color( unsigned char r, unsigned char g, unsigned char b) noexcept
        : dword((r << 16u) | (g << 8u) | b)
        { }
        constexpr Color(Color col, unsigned char x) noexcept
        : Color((x << 24u) | col.dword)
        { }

        Color& operator=(Color col) noexcept
        {
            dword = col.dword;
            return *this;
        }

        constexpr unsigned char GetX() const noexcept { return dword >> 24u; }
        constexpr unsigned char GetA() const noexcept { return GetX(); }
        constexpr unsigned char GetR() const noexcept { return (dword >> 16u) & 0xFFu; }
        constexpr unsigned char GetG() const noexcept { return (dword >> 8u) & 0xFFu; }
        constexpr unsigned char GetB() const noexcept { return dword & 0xFFu; }
        void SetX(unsigned char x) noexcept { dword = (dword & 0xFFFFFFu) | (x << 24u); }
        void SetA(unsigned char a) noexcept { SetX(a); }
        void SetR(unsigned char r) noexcept { dword = (dword & 0xFFFFFFu) | (r << 16u); }
        void SetG(unsigned char g) noexcept { dword = (dword & 0xFFFFFFu) | (g << 8u); }
        void SetB(unsigned char b) noexcept { dword = (dword & 0xFFFFFFu) | b; }
    };

public:
    class Exception : public ChiliException
    {
    public:
        Exception(int line, const char *file, std::string note) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        const std::string& GetNote() const noexcept;

    private:
        std::string m_note;
    };

public:
    Surface(unsigned int width, unsigned int height, unsigned int pitch) noexcept;
    Surface(unsigned int width, unsigned int height) noexcept;
    Surface(Surface &&s) noexcept;
    Surface(Surface &s) = delete;
    ~Surface();

    Surface &operator=(Surface &&donor) noexcept;
    Surface &operator=(const Surface &s) = delete;

    void Clear(Color fill_value) noexcept;
    void PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG);
    Color GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG);
    unsigned int GetWidth() const noexcept;
    unsigned int GetHeight() const noexcept;
    Color* GetBufferPtr() noexcept;
    const Color* GetBufferPtr() const noexcept;
    const Color* GetBufferPtrConst() const noexcept;
    static Surface FromFile(const std::string& filename);
    void Save(const std::string& filename) const;
    void Copy(const Surface &s) noexcept(!IS_DEBUG);

private:
    Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept;

    std::unique_ptr<Color[]> m_pBuffer;
    unsigned int m_width;
    unsigned int m_height;
};



#endif //__SURFACE_H
