#include <windows.h>
#include "Utils.hpp"
#include "Window.h"
#include <sstream>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        Window wnd( 800,300,"Donkey Fart Box" );

        MSG msg;
        BOOL gResult;
        while( (gResult = GetMessage( &msg,nullptr,0,0 )) > 0 )
        {
            // TranslateMessage will post auxilliary WM_CHAR messages from key msgs
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            while (!wnd.m_mouse.IsEmpty())
            {
                const auto e = wnd.m_mouse.Read();
                if (e.GetType() == Mouse::Event::Type::MOVE)
                {
                    std::ostringstream oss;
                    oss << "Mouse Position: (" << e.GetX() << ", " << e.GetY() << ")";
                    wnd.SetTitle(oss.str());
                }
            }
         }

        // check if GetMessage call itself borked
        if( gResult == -1 )
            return -1;

        // wParam here is the value passed to PostQuitMessage
        return static_cast<int>(msg.wParam);
    }
    catch( const ChiliException& e )
    {
        MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION );
    }
    catch( const std::exception& e )
    {
        MessageBox( nullptr,e.what(),"Standard Exception",MB_OK | MB_ICONEXCLAMATION );
    }
    catch( ... )
    {
        MessageBox( nullptr,"No details available","Unknown Exception",MB_OK | MB_ICONEXCLAMATION );
    }
    return -1;


}

