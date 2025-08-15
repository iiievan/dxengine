#include "Graphics.h"
#include <sstream>
#include "Utils.hpp"
#include "dxerr.h"

#pragma comment(lib, "d3d11.lib")

#define GFX_THROW_FAILED(hrcall) \
    if (FAILED(hr = (hrcall)))   \
    throw Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))

const char *Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec << " ("
        << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl
        << GetOriginString();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    return DXGetErrorString(m_hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
    char buf[512];
    DXGetErrorDescription(m_hr, buf, sizeof(buf));
    return buf;
}

const char *Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

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
    swchd.OutputWindow = (HWND)6969696;                                                //
    swchd.Windowed = TRUE;                                                    //  no fullscreen
    swchd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              //
    swchd.Flags = 0;                                                          //

    // for checking results of d3d functions
    HRESULT hr;

    GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swchd,
        &m_pSwapChain,
        &m_pDevice,
        nullptr,
        &m_pContext));

    ID3D11Resource *pBackBuffer = nullptr;
    GFX_THROW_FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void **>(&pBackBuffer)));
    GFX_THROW_FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pTargetView));
    ReleaseCOM(pBackBuffer);
}

Graphics::~Graphics()
{
    if (m_pTargetView != nullptr)
        ReleaseCOM(m_pTargetView);

    if (m_pContext != nullptr)
        ReleaseCOM(m_pContext);

    if (m_pSwapChain != nullptr)
        ReleaseCOM(m_pSwapChain);

    if (m_pDevice != nullptr)
        ReleaseCOM(m_pDevice);
}

void Graphics::EndFrame()
{
    HRESULT hr;
    if (FAILED(hr = m_pSwapChain->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
            throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
        else
            GFX_THROW_FAILED(hr);
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = {red, green, blue, 1.0f};
    m_pContext->ClearRenderTargetView(m_pTargetView, color);
}