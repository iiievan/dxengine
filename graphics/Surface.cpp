#define FULL_WINTARD
#include "Surface.h"
#include <algorithm>

namespace Gdiplus
{
using std::max;
using std::min;
} // namespace Gdiplus

#include <gdiplus.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

// surface exception stuff
Surface::Exception::Exception(int line, const char *file, std::string note) noexcept
    : ChiliException(line, file), m_note(std::move(note))
{
}

const char *Surface::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << ChiliException::what() << std::endl << "[Note] " << GetNote();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char *Surface::Exception::GetType() const noexcept
{
    return "Chili Graphics Exception";
}

const std::string &Surface::Exception::GetNote() const noexcept
{
    return m_note;
}

Surface::Surface(unsigned int width, unsigned int height) noexcept
    : m_pBuffer(std::make_unique<Color[]>(width * height)), m_width(width), m_height(height)
{
}

Surface::Surface(Surface &&s) noexcept : m_pBuffer(std::move(s.m_pBuffer)), m_width(s.m_width), m_height(s.m_height) {}

Surface::~Surface() {}

Surface &Surface::operator=(Surface &&donor) noexcept
{
    m_pBuffer = std::move(donor.m_pBuffer);
    m_width = donor.m_width;
    m_height = donor.m_height;
    donor.m_pBuffer = nullptr;
    return *this;
}

void Surface::Clear(Color fill_value) noexcept
{
    memset(m_pBuffer.get(), fill_value.dword, m_width * m_height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!true)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < m_width);
    assert(y < m_height);
    m_pBuffer[y * m_width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!true)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < m_width);
    assert(y < m_height);
    return m_pBuffer[y * m_width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
    return m_width;
}

unsigned int Surface::GetHeight() const noexcept
{
    return m_height;
}

Surface::Color *Surface::GetBufferPtr() noexcept
{
    return m_pBuffer.get();
}

const Surface::Color *Surface::GetBufferPtr() const noexcept
{
    return m_pBuffer.get();
}

const Surface::Color *Surface::GetBufferPtrConst() const noexcept
{
    return m_pBuffer.get();
}

Surface Surface::FromFile(const std::string &filename)
{
    unsigned int             width = 0;
    unsigned int             height = 0;
    std::unique_ptr<Color[]> pBuffer;
    bool alphaLoaded = false;

    {
        // convert filename to wide string (for GDIPlus)
        wchar_t widename[512];
        mbstowcs_s(nullptr, widename, filename.c_str(), _TRUNCATE);

        Gdiplus::Bitmap bitmap(widename);
        if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
        {
            std::stringstream ss;
            ss << "Loading image [" << filename << "]: failed to load.";
            throw Exception(__LINE__, __FILE__, ss.str());
        }

        height = bitmap.GetHeight();
        width = bitmap.GetWidth();
        pBuffer = std::make_unique<Color[]>(width * height);
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                Gdiplus::Color c;
                bitmap.GetPixel(x, y, &c);
                pBuffer[y * width + x] = c.GetValue();
                if (c.GetAlpha() != 255)
                    alphaLoaded = true;
            }
        }
    }

    return Surface(width, height, std::move(pBuffer), alphaLoaded);
}

void Surface::Save(const std::string &filename) const
{
    auto GetEncoderClsid = [&filename](const WCHAR *format, CLSID *pClsid) -> void {
        UINT num = 0;  // number of image encoders
        UINT size = 0; // size of image encoder array in bytes

        Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0)
        {
            std::stringstream ss;
            ss << "Saving Surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
            throw Exception(__LINE__, __FILE__, ss.str());
        }

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT i = 0; i < num; i++)
        {
            if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[i].Clsid;
                free(pImageCodecInfo);
                return;
            }
        }
        free(pImageCodecInfo);

        std::stringstream ss;
        ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
        throw Exception(__LINE__, __FILE__, ss.str());
    };

    CLSID bmpID;
    GetEncoderClsid(L"image/bmp", &bmpID);

    wchar_t widename[512];
    mbstowcs_s(nullptr, widename, filename.c_str(), _TRUNCATE);

    Gdiplus::Bitmap bitmap(m_width, m_height, m_width * sizeof(Color), PixelFormat32bppARGB, (BYTE *)m_pBuffer.get());

    if (bitmap.Save(widename, &bmpID, nullptr) != Gdiplus::Status::Ok)
    {
        std::stringstream ss;
        ss << "Saving surface to [" << filename << "]: failed to save.";
        throw Exception(__LINE__, __FILE__, ss.str());
    }
}

void Surface::Copy(const Surface &s) NOXND
{
    assert(m_width == s.m_width);
    assert(m_height == s.m_height);
    memcpy(m_pBuffer.get(), s.m_pBuffer.get(), m_width * m_height * sizeof(Color));
}

bool Surface::AlphaLoaded() const noexcept
{
    return m_alphaLoaded;
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded) noexcept
: m_width(width), m_height(height), m_pBuffer(std::move(pBufferParam)), m_alphaLoaded(alphaLoaded)
{
}