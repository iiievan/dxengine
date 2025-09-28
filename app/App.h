#ifndef __APP_H
#define __APP_H
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "geometry/Mesh.h"


#define MAIN_SCREEN_WIDTH (1280)
#define MAIN_SCREEN_HEIGHT (720)

class App
{
public:
    App();
    ~App();

    // master frame/message loop
    int Go();

private:
    void DoFrame();
    void ShowImguiDemoWindow();
    void ShowRawInputWindow();

private:
    bool m_CursorEnabled = true;
    int m_x = 0;
    int m_y = 0;
    ImguiManager m_imgui;
    Window m_wnd;
    ChiliTimer m_timer;
    Camera m_camera;
    PointLight m_pointlight;

    float m_speed_factor {1.0f};
    Model m_nanosuit{m_wnd.Gfx(),"models\\nanosuit.gltf"};
};

#endif // __APP_H
