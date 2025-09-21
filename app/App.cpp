#include "App.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <memory>
#include "ChiliMath.h"
#include "GDIPlusManager.h"
#include "Surface.h"
#include "VertexLayout.h"
#include "bindable/VertexBuffer.h"
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

void f()
{
    VertexLayoutBuffer vlb(std::move(VertexLayout{}.Append<VertexLayout::Position3D>()
                                                  .Append<VertexLayout::Normal>()
                                                  .Append<VertexLayout::Texture2D>()));
    vlb.EmplaceBack(dx::XMFLOAT3{1.0f,1.0f,5.0f},
                    dx::XMFLOAT3{2.0f,1.0f,4.0f},
                    dx::XMFLOAT2{6.0f,9.0f});

    vlb.EmplaceBack(dx::XMFLOAT3{6.0f,9.0f,6.0f},
                    dx::XMFLOAT3{9.0f,6.0f,4.0f},
                    dx::XMFLOAT2{4.2f,1.0f});

    auto pos = vlb[0].Attr<VertexLayout::Position3D>();
    auto norm = vlb[0].Attr<VertexLayout::Normal>();
    auto tex = vlb[1].Attr<VertexLayout::Texture2D>();
    vlb.Back().Attr<VertexLayout::Position3D>().z = 420.0f;
    pos = vlb.Back().Attr<VertexLayout::Position3D>();
}

App::App()
:m_wnd(800, 600, "Donkey Fart Box"),
 m_pointlight(m_wnd.Gfx())
{
    PrintAssimpVersion();

    f();
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
            switch( sdist( rng ) )
            {
                case 0:
                    return std::make_unique<Box>(gfx, rng, adist, ddist, odist, rdist, bdist, mat);
                case 1:
                    return std::make_unique<Cylinder>(gfx, rng, adist, ddist, odist, rdist, bdist, tdist);
                case 2:
                    return std::make_unique<Pyramid>(gfx, rng, adist, ddist, odist, rdist, tdist);
                case 3:
                    return std::make_unique<SkinnedBox>(gfx, rng, adist, ddist, odist, rdist);
                case 4:
                    return std::make_unique<AssTest>(gfx, rng, adist, ddist, odist, rdist, mat, 1.5f);
                default:
                    assert( false && "impossible drawable option in factory" );
                    return {};
            }
        }
    private:
        Graphics& gfx;
        std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<int> sdist{ 0,4 };
        std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
        std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
        std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
        std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
        std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
        std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
        std::uniform_int_distribution<int> tdist{ 3,30 };
    };

    m_drawables.reserve(m_nDrawables);
    std::generate_n(std::back_inserter(m_drawables), m_nDrawables, Factory {m_wnd.Gfx()});

    //init box pointers for editing instance parameters
    for (auto &pd : m_drawables)
    {
        if(auto pb = dynamic_cast<Box *>(pd.get()))
            m_boxes.push_back(pb);
    }

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

    // imgui windows
    SpawnSimulationWindow();
    m_camera.SpawnControlWindow();
    m_pointlight.SpawnControlWindow();
    SpawnBoxWindowManagerWindow();
    SpawnBoxWindows();

    // present
    m_wnd.Gfx().EndFrame();
}
void App::SpawnSimulationWindow() noexcept
{
    if( ImGui::Begin( "Simulation Speed" ) )
    {
        ImGui::SliderFloat( "Speed Factor",&m_speed_factor,0.0f,6.0f,"%.4f",3.2f );
        ImGui::Text( "%.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
        ImGui::Text( "Status: %s",m_wnd.m_kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (hold spacebar to pause)" );
    }
    ImGui::End();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
    if( ImGui::Begin( "Boxes" ) )
    {
        using namespace std::string_literals;
        const auto preview = m_comboBoxIndex ? std::to_string( *m_comboBoxIndex ) : "Choose a box..."s;
        if( ImGui::BeginCombo( "Box Number",preview.c_str() ) )
        {
            for( int i = 0; i < m_boxes.size(); i++ )
            {
                const bool selected = m_comboBoxIndex && (*m_comboBoxIndex == i);
                if( ImGui::Selectable( std::to_string( i ).c_str(),selected ) )
                    m_comboBoxIndex = i;

                if( selected )
                     ImGui::SetItemDefaultFocus();

            }
            ImGui::EndCombo();
        }
        if( ImGui::Button( "Spawn Control Window" ) && m_comboBoxIndex )
        {
            m_boxControlIds.insert( *m_comboBoxIndex );
            m_comboBoxIndex.reset();
        }
    }
    ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
    for( auto i = m_boxControlIds.begin(); i != m_boxControlIds.end(); )
    {
        if( !m_boxes[*i]->SpawnControlWindow( *i,m_wnd.Gfx() ) )
            i = m_boxControlIds.erase( i );
        else
            i++;
    }
}