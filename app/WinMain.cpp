#include <windows.h>
#include <sstream>
#include "Utils.hpp"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        Window wnd(800, 600, "Donkey Fart Box");

        MSG  msg;
        BOOL gResult;
        while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            // TranslateMessage will post auxilliary WM_CHAR messages from key msgs
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // test wheel delta mouse
            static int i = 0;
            while (!wnd.m_mouse.IsEmpty())
            {
                const auto e = wnd.m_mouse.Read();
                switch (e.GetType())
                {
                    case Mouse::Event::Type::WHEEL_UP:
                        i++;
                        {
                            std::ostringstream oss;
                            oss << "Up:" << i;
                            wnd.SetTitle(oss.str());
                        }
                        break;
                    case Mouse::Event::Type::WHEEL_DOWN:
                        i--;
                        {
                            std::ostringstream oss;
                            oss << "Down:" << i;
                            wnd.SetTitle(oss.str());
                        }
                        break;
                }
            }
        }

        // check if GetMessage call itself borked
        if (gResult == -1)
            return -1;

        // wParam here is the value passed to PostQuitMessage
        return static_cast<int>(msg.wParam);
    }
    catch (const ChiliException &e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception &e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}
