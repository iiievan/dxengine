#include "Window.h"
#include <sstream>
#include "../resources/resources.h"

#define APP_ICON IDI_CUBE_ICON

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
    : hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 128, 128, 0));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 64, 64, 0));
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

Window::Exception::Exception(int line, const char *file, HRESULT hr) noexcept
    : ChiliException(line, file),
      m_hr(hr)
{
}

const char *Window::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code]" << GetErrorCode() << std::endl
        << "[Description]" << GetErrorString() << std::endl
        << GetOriginString();

    m_whatBuffer = oss.str().c_str();

    return m_whatBuffer.c_str();
}

const char *Window::Exception::GetType() const noexcept
{
    return "Chili Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    char *pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0,
        nullptr);

    if (nMsgLen == 0)
        return "Undefined error code";

    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);

    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept { return m_hr; }

std::string Window::Exception::GetErrorString() const noexcept
{
    return TranslateErrorCode(m_hr);
}

const char *Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

// Window Stuff
Window::Window(int width, int height, const char *name)
: m_width(width), m_height(height)
{
    s_windowCount++;
    // calculate window size based on desired client region size
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
        throw CHWND_LAST_EXCEPT();

    // create window & get hWnd
    m_hWnd = CreateWindow(
        WindowClass::GetName(),
        name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        WindowClass::GetInstance(),
        this); // here we send a pointer for CREATESTRUCTW

    if (m_hWnd == nullptr)
        throw CHWND_LAST_EXCEPT();

    // show window
    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    if (m_hWnd != nullptr)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

void Window::SetTitle(const std::string &title)
{
    if (SetWindowText(m_hWnd, title.c_str()) == 0)
        throw CHWND_LAST_EXCEPT();
}

std::optional<WPARAM> Window::ProcessMessages()
{
    MSG msg;
    // while queue has messages, remove and dispatch them (but do not block on empty queue)
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            // return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
            return msg.wParam;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // return empty optional when not quitting app
    return {};
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
        Window *const              pWnd = static_cast<Window *>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window class handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCCREATE message, handle with default handler
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // retrieve ptr to window class
    Window *const pWnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window class handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
        case WM_CLOSE:
            DestroyWindow(m_hWnd);
            m_hWnd = nullptr; // предупреждаем повторное уничтожение окна в деструкторе.
            if (--s_windowCount == 0)
                PostQuitMessage(0); // Закрыть приложение, если это было последнее окно
            return 0;               // destroy window once by Destructor: Window::~Window()
        case WM_KILLFOCUS:
            m_kbd.ClearState();
            break;
        /********* Keyboard handle *********/
        case WM_KEYDOWN:
        // syskey commands need to be handled to track ALT key (VK_MENU) and F10
        case WM_SYSKEYDOWN:
            if (!(lParam & 0x40000000) || m_kbd.AutorepeatIsEnabled())
                m_kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            m_kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
        case WM_CHAR:
            m_kbd.OnChar(static_cast<unsigned char>(wParam));
            break;
        /********* Keyboard handle *********/
        /*********** Mouse handle **********/
        case WM_MOUSEMOVE:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            // if cursor in client region -> log move, and log enter + capture mouse (if not previously in window)
            if (pt.x > 0 && pt.x < m_width && pt.y > 0 && pt.y < m_height )
            {
                m_mouse.OnMouseMove(pt.x, pt.y);
                if (!m_mouse.IsInWindow())
                {
                    SetCapture(hWnd);
                    m_mouse.OnMouseEnter();
                }
            }
            //if cursor not in client -> log move / maintain capture if button down
            else
            {
                if (wParam & (MK_LBUTTON | MK_RBUTTON))
                    m_mouse.OnMouseMove(pt.x, pt.y);
                else
                {
                    ReleaseCapture();   // buttons L and R released -> we are not draggind by mouse!
                    m_mouse.OnMouseLeave();
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            m_mouse.OnLeftPressed(pt.x,pt.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            m_mouse.OnRightPressed(pt.x,pt.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            m_mouse.OnLeftReleased(pt.x,pt.y);

            if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height )
            {
                ReleaseCapture();   // button L released -> we are not draggind by mouse!
                m_mouse.OnMouseLeave();
            }
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            m_mouse.OnRightReleased(pt.x,pt.y);
            if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height )
            {
                ReleaseCapture();   // button R released -> we are not draggind by mouse!
                m_mouse.OnMouseLeave();
            }
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS pt = MAKEPOINTS( lParam );
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            m_mouse.OnWheelDelta(pt.x,pt.y,delta);
            break;
        }
        /*********** Mouse handle **********/
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}