#include <windows.h>
#include "Utils.hpp"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Window wnd(800, 600, "DxEngine");
    Window wnd2(640, 480, "SetUp");

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

