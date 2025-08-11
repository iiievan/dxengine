#include <windows.h>

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const auto pAppName = "DxEngine";

    // register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = pAppName;
    wc.hIconSm = nullptr;

    RegisterClassEx(&wc);

    // create window instance
    HWND hwnd = CreateWindowEx(0,
                               pAppName,
                               "Engine main window.",
                               WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                               200,
                               200,
                               640,
                               480,
                               nullptr,
                               nullptr,
                               hInstance,
                               nullptr);

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    BOOL gResult;
    while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (gResult == -1)
        return -1;

    return static_cast<int>(msg.wParam);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            PostQuitMessage(69);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
