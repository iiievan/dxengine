#include "App.h"
#include "drawable/Box.h"
#include "drawable/Melon.h"
#include "drawable/Pyramid.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"

GDIPlusManager gdipm;

#define ever (;;)

App::App()
:m_wnd(800, 600, "Donkey Fart Box")
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
            switch( typedist( rng ) )
            {
                case 0:
                    return std::make_unique<Pyramid>(
                        gfx,rng,adist,ddist,
                        odist,rdist
                    );
                case 1:
                    return std::make_unique<Box>(
                        gfx,rng,adist,ddist,
                        odist,rdist,bdist
                    );
                case 2:
                    return std::make_unique<Melon>(
                        gfx,rng,adist,ddist,
                        odist,rdist,longdist,latdist
                    );
                default:
                    assert( false && "bad drawable type in factory" );
                    return {};
            }
        }
    private:
        Graphics& gfx;
        std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
        std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
        std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
        std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
        std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
        std::uniform_int_distribution<int> latdist{ 5,20 };
        std::uniform_int_distribution<int> longdist{ 10,40 };
        std::uniform_int_distribution<int> typedist{ 0,2 };
    };

    Factory f( m_wnd.Gfx() );
    m_drawables.reserve( m_nDrawables );
    std::generate_n( std::back_inserter( m_drawables ),m_nDrawables,f );

    const auto s = Surface::FromFile( "images\\kappa50.png" );

    m_wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
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
    const auto dt = m_timer.Mark();
    m_wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
    for (auto &d : m_drawables)
    {
        d->Update(dt);
        d->Draw(m_wnd.Gfx());
    }
    m_wnd.Gfx().EndFrame();
}