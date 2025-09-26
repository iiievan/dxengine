#ifndef __APP_H
#define __APP_H
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "geometry/Mesh.h"
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
    void ShowModelWindow();

private:
    ImguiManager m_imgui;
    Window m_wnd;
    ChiliTimer m_timer;
    Camera m_camera;
    PointLight m_pointlight;

    float m_speed_factor {1.0f};
    Model m_nanosuit{m_wnd.Gfx(),"models\\nanosuit.obj"};
    struct
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } m_pos;
};

#endif // __APP_H
