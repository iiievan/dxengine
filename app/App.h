#ifndef __APP_H
#define __APP_H
#include "Window.h"
#include "ChiliTimer.h"

class App
{
public:
    App();
    int Go();

private:
    void DoFrame();
private:
    Window m_wnd;
    ChiliTimer m_timer;
};

#endif // __APP_H
