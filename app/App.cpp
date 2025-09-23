#include "App.h"
#include <algorithm>
#include <memory>
#include "ChiliMath.h"
#include "GDIPlusManager.h"
#include "Surface.h"
#include "drawable/AssTest.h"
#include "drawable/Box.h"
#include "drawable/Cylinder.h"
#include "drawable/Pyramid.h"
#include "drawable/SkinnedBox.h"
#include "imgui.h"

namespace dx = DirectX;
GDIPlusManager gdipm;

#define ever (;;)

#include <assimp/version.h> // Главный файл с информацией о версии

void PrintAssimpVersion()
{
   int major, minor, rev;
         major = aiGetVersionMajor(),
         minor = aiGetVersionMinor(),
         rev =   aiGetVersionRevision();
    printf("Assimp Version: %d:%d:%d\n",major,minor,rev);
}

App::App()
:m_wnd(800, 600, "Donkey Fart Box"),
 m_pointlight(m_wnd.Gfx())
{
    PrintAssimpVersion();

    m_wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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

    const auto transform = dx::XMMatrixRotationRollPitchYaw(m_pos.roll, m_pos.pitch, m_pos.yaw) *
                                     dx::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    m_nanosuit.Draw(m_wnd.Gfx(), transform);
    m_pointlight.Draw(m_wnd.Gfx());

    // imgui windows
    m_camera.SpawnControlWindow();
    m_pointlight.SpawnControlWindow();
    ShowModelWindow();

    // present
    m_wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
    if (ImGui::Begin("Model"))
    {
        using namespace std::string_literals;
        ImGui::Text("Orientation");
        ImGui::SliderAngle("Roll", &m_pos.roll, -180.0f, 180.0f);
        ImGui::SliderAngle("Pitch", &m_pos.pitch, -180.0f, 180.0f);
        ImGui::SliderAngle("Yaw", &m_pos.yaw, -180.0f, 180.0f);

        ImGui::Text("Position");
        ImGui::SliderFloat("X", &m_pos.x, -20.0f, 20.0f);
        ImGui::SliderFloat("Y", &m_pos.y, -20.0f, 20.0f);
        ImGui::SliderFloat("Z", &m_pos.z, -20.0f, 20.0f);
    }
    ImGui::End();
}

