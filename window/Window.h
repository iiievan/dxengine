#ifndef __WINDOW_H
#define __WINDOW_H

#include "WinDefs.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>


class Window
{
public:
    class Exception : public ChiliException
    {
        using ChiliException::ChiliException;
    public:
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
    };

    class HrException : public Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr) noexcept
        :Exception(line,file),m_hr(hr)
        {}
        const char* what() const noexcept override;
        const char* GetType() const noexcept override   { return "Chili Window Exception"; }
        HRESULT GetErrorCode() const noexcept           { return m_hr; }
        std::string GetErrorDescription() const noexcept;
    private:
        HRESULT m_hr;
    };

    class NoGfxException : public Exception
    {
    public:
        using Exception::Exception;
        const char* GetType() const noexcept override { return "Chili Window Exception [No Graphics]";}
    };

private:
    // singleton manages registration/cleanup of window class
    class WindowClass
    {
    public:
        static const char *GetName() noexcept;
        static HINSTANCE   GetInstance() noexcept;

    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(const WindowClass &) = delete;
        WindowClass                 &operator=(const WindowClass &) = delete;
        static constexpr const char *wndClassName = "Direct3D Engine Window";
        static WindowClass           wndClass;
        HINSTANCE                    hInst;
    };

public:
    Window(int width, int height, const char *name);
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    void SetTitle(const std::string &title);
    static std::optional<WPARAM> ProcessMessages() noexcept;
    Graphics& Gfx();

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT                 HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
    Keyboard m_kbd;
    Mouse m_mouse;
private:
    int  m_width;
    int  m_height;
    HWND m_hWnd;
    static inline int s_windowCount = 0;

    std::unique_ptr<Graphics> m_pGfx;
};

#endif // __WINDOW_H