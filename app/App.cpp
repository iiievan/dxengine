#include "App.h"
#include "drawable/Box.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui.h"

namespace dx = DirectX;
GDIPlusManager gdipm;

#define ever (;;)

App::App()
:m_wnd(800, 600, "Donkey Fart Box"),
 m_pointlight(m_wnd.Gfx())
{
    class Factory
    {
    public:
        Factory( Graphics& gfx )
            :
            gfx( gfx )
        {}

        std::unique_ptr<Drawable> operator()()
        {
            const DirectX::XMFLOAT3 mat = { cdist( rng ),cdist( rng ),cdist( rng ) };
            return std::make_unique<Box>(
                gfx,rng,adist,ddist,
                odist,rdist,bdist,mat
            );
        }
    private:
        Graphics& gfx;
        std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
        std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
        std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
        std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
        std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
        std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
    };

    m_drawables.reserve(m_nDrawables);
    std::generate_n(std::back_inserter(m_drawables), m_nDrawables, Factory {m_wnd.Gfx()});

    m_wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{}

int App::Go()
{
   for
       ever
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

    for (auto &d : m_drawables)
    {
        d->Update(m_wnd.m_kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
        d->Draw(m_wnd.Gfx());
    }
    m_pointlight.Draw(m_wnd.Gfx());

    // imgui window to control simulation speed
    if (ImGui::Begin("Simulation Speed"))
    {
        ImGui::SliderFloat("Speed factor", &m_speed_factor, 0.0f, 6.0f,"%.4f",3.2f);
        ImGui::Text(" %.3f ms/frame (%.1f FPS)",1000.0f/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Status: %s",m_wnd.m_kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
    }
    ImGui::End();

    // imgui window to control camera and light
    m_camera.SpawnControlWindow();
    m_pointlight.SpawnControlWindow();

    // present
    m_wnd.Gfx().EndFrame();
}