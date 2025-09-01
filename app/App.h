#ifndef __APP_H
#define __APP_H
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"

class App
{
public:
    App();
    ~App();

    // master frame/message loop
    int Go();

private:
    void DoFrame();
private:
    ImguiManager m_imgui;
    Window m_wnd;
    ChiliTimer m_timer;
    std::vector<std::unique_ptr<class Box>> boxes;
    std::vector<std::unique_ptr<class Drawable>> m_drawables;
    static constexpr size_t m_nDrawables = 180;
};

#endif // __APP_H
