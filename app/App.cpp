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
    const float t = m_timer.Peek();
    std::ostringstream oss;
    oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
    m_wnd.SetTitle(oss.str());
}