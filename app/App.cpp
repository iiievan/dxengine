#include "App.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "bindable/VertexBuffer.h"

namespace dx = DirectX;
GDIPlusManager gdipm;

#define ever (;;)

#include <assimp/version.h> // Главный файл с информацией о версии

void PrintAssimpVersion()
{
   unsigned int major, minor, rev;
         major = aiGetVersionMajor(),
         minor = aiGetVersionMinor(),
         rev =   aiGetVersionRevision();
    printf("Assimp Version: %d:%d:%d\n",major,minor,rev);
}

App::App()
:m_wnd(MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT, "Donkey Fart Box"),
 m_pointlight(m_wnd.Gfx())
{
    PrintAssimpVersion();

    //m_wall.SetRootTransform(dx::XMMatrixTranslation(-1.5f,0.0f,0.0f));
    //m_tp.SetPos({1.5f,0.0f,0.0f});
    m_wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 50.0f));
}

App::~App()
{}

int App::Go()
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
    const auto dt = m_timer.Mark() * m_speed_factor;

    m_wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
    m_wnd.Gfx().SetCamera(m_camera.GetMatrix());
    m_pointlight.Bind(m_wnd.Gfx(), m_camera.GetMatrix());

    //m_wall.Draw(m_wnd.Gfx());
    //m_tp.Draw(m_wnd.Gfx());
    //m_nanosuit.Draw(m_wnd.Gfx());
    //m_Gobber.Draw(m_wnd.Gfx());
    //m_Dwarf.Draw(m_wnd.Gfx());
    m_backpack.Draw(m_wnd.Gfx());
    m_pointlight.Draw(m_wnd.Gfx());


    while (const auto e = m_wnd.kbd.ReadKey())
    {
        if (!e->isPress())
            continue;

        switch (e->getCode())
        {
            case VK_ESCAPE:
                if (m_wnd.IsCursorEnabled())
                {
                    m_wnd.DisableCursor();
                    m_wnd.mouse.EnableRaw();
                }
                else
                {
                    m_wnd.EnableCursor();
                    m_wnd.mouse.DisableRaw();
                }
                break;
            case VK_F1:
                m_showDemoWindow = true;
                break;
        }
    }

    if (!m_wnd.IsCursorEnabled())
    {
        if (m_wnd.kbd.KeyIsPressed('W'))
            m_camera.Translate({0.0f, 0.0f, dt});
        if (m_wnd.kbd.KeyIsPressed('A'))
            m_camera.Translate({-dt, 0.0f, 0.0f});
        if (m_wnd.kbd.KeyIsPressed('S'))
            m_camera.Translate({0.0f, 0.0f, -dt});
        if (m_wnd.kbd.KeyIsPressed('D'))
            m_camera.Translate({dt, 0.0f, 0.0f});
        if (m_wnd.kbd.KeyIsPressed('R'))
            m_camera.Translate({0.0f, dt, 0.0f});
        if (m_wnd.kbd.KeyIsPressed('F'))
            m_camera.Translate({0.0f, -dt, 0.0f});
    }

    while (const auto delta = m_wnd.mouse.ReadRawDelta())
    {
        if (!m_wnd.IsCursorEnabled())
            m_camera.Rotate((float)delta->x, (float)delta->y);
    }

    // imgui windows
    m_camera.SpawnControlWindow();
    m_pointlight.SpawnControlWindow();
    ShowImguiDemoWindow();
    //m_nanosuit.ShowWindow("Suit #1");
    //m_wall.ShowWindow("Wall");
    //m_tp.SpawnControlWindow(m_wnd.Gfx());
    //m_Gobber.ShowWindow(m_wnd.Gfx(), "Gobber");
    //m_Dwarf.ShowWindow(m_wnd.Gfx(), "Dwarf");
    m_backpack.ShowWindow(m_wnd.Gfx(), "Backpack");

    // present
    m_wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
    if (m_showDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }
}

