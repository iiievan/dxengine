#include "Graphics.h"
#include "Utils.hpp"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC swchd = {};
    swchd.BufferDesc.Width = 0;                               // if Width and Height not set
    swchd.BufferDesc.Height = 0;                              // it will use hWnd to set window proportions
    swchd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;     //
    swchd.BufferDesc.RefreshRate.Numerator = 0;               //
    swchd.BufferDesc.RefreshRate.Denominator = 0;             //
    swchd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // if Width and Height not set no need scaling
    swchd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //
    swchd.SampleDesc.Count = 1;                                               //
    swchd.SampleDesc.Quality = 0;                                             //
    swchd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                      //
    swchd.BufferCount = 1;                                                    // one Back buffer
    swchd.OutputWindow = hWnd;                                                //
    swchd.Windowed = TRUE;                                                    //  no fullscreen
    swchd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              //
    swchd.Flags = 0;                                                          //

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swchd,
        &m_pSwapChain,
        &m_pDevice,
        nullptr,
        &m_pContext);

    ID3D11Resource *pBackBuffer = nullptr;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    m_pDevice->CreateRenderTargetView(pBackBuffer,nullptr,&m_pTarget);
    ReleaseCOM(pBackBuffer);
}

Graphics::~Graphics()
{
    if (m_pTarget != nullptr)
        ReleaseCOM(m_pTarget);

    if (m_pContext != nullptr)
        ReleaseCOM(m_pContext);

    if (m_pSwapChain != nullptr)
        ReleaseCOM(m_pSwapChain);

    if (m_pDevice != nullptr)
        ReleaseCOM(m_pDevice);
}

void Graphics::EndFrame()
{
    m_pSwapChain->Present(1u, 0u);
}