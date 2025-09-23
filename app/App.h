#ifndef __APP_H
#define __APP_H
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "geometry/Model.h"
#include <set>


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
    Camera m_camera;
    PointLight m_pointlight;

    float m_speed_factor {1.0f};
    Model m_nanosuit{m_wnd.Gfx(),"models\\nanosuit.obj"};
};

#endif // __APP_H
