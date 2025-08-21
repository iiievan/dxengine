#include "App.h"
#include <sstream>
#include <iomanip>

#define ever (;;)

App::App()
:m_wnd(800, 600, "Donkey Fart Box")
{ }

int  App::Go()
{
   for ever
   {
       // process all messages pending, but to not block for new messages
       if (const auto exitcode = Window::ProcessMessages())
       {
           // if return optional has value, means we're quitting so return exit code
           return *exitcode;
       }
       DoFrame();
   }
}

void App::DoFrame()
{
    const float c = sin(m_timer.Peek())/2.0f + 0.5f;
    m_wnd.Gfx().ClearBuffer(c,c,1.0f);
    m_wnd.Gfx().DrawTestTriangle(m_timer.Peek());
    m_wnd.Gfx().EndFrame();
}