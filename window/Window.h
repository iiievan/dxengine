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
    public:
        Exception(int line, const char *file, HRESULT hr) noexcept;
        const char *what() const noexcept override;
        virtual const char *GetType() const noexcept;
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
    private:
        HRESULT m_hr;
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
    static std::optional<WPARAM> ProcessMessages();
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

#define  CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__, hr)
#define  CHWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())

#endif // __WINDOW_H