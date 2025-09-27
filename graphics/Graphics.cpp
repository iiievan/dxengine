#include "Graphics.h"
#include <d3dcompiler.h>
#include <sstream>
#include "Utils.hpp"
#include "dxerr.h"
#include <directxmath.h>
#include "GraphicsThrowMacroses.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::HrException::HrException(int line, const char *file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
    : Exception(line, file), m_hr(hr)
{
    for (const auto &m : infoMsgs)
    {
        m_info += m;
        m_info.push_back('\n');
    }

    // remove final new line if exist
    if (!m_info.empty())
        m_info.pop_back();
}

const char *Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec << " ("
        << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if (!m_info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
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
std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return m_info;
}
Graphics::InfoException::InfoException(int line, const char *file, std::vector<std::string> infoMsgs) noexcept
    : Exception(line, file)
{
    // join all info messages with newlines into single string
    for (const auto &m : infoMsgs)
    {
        m_info += m;
        m_info.push_back('\n');
    }

    // remove final new line if exist
    if (!m_info.empty())
        m_info.pop_back();
}

const char *Graphics::InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char *Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::Graphics(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swchd = {};
    swchd.BufferDesc.Width = width;                               // if Width and Height not set
    swchd.BufferDesc.Height = height;                              // it will use hWnd to set window proportions
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

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // for checking results of d3d functions
    HRESULT hr;

    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swchd,
        &m_pSwapChain,
        &m_pDevice,
        nullptr,
        &m_pContext));

    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    GFX_THROW_INFO(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GFX_THROW_INFO(m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_pTargetView));

    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc = D3D11_COMPARISON_LESS;

    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    GFX_THROW_INFO(m_pDevice->CreateDepthStencilState(&dsd, &pDSState));
    m_pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

    // create z-buffer(depth stencil) texture
    wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC         descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    GFX_THROW_INFO(m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

    // create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
    dsvd.Format = DXGI_FORMAT_D32_FLOAT;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0u;
    GFX_THROW_INFO(m_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &m_pDSView));

    // bind depth stecil view to Output Merger
    m_pContext->OMSetRenderTargets(1u, m_pTargetView.GetAddressOf(), m_pDSView.Get());

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_pContext->RSSetViewports(1u, &vp);

    // init imgui d3d impl
    ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());
}

Graphics::~Graphics()
{
    ImGui_ImplDX11_Shutdown();
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
    if (m_imguiEnabled)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    const float color[] = {red, green, blue, 1.0f};
    m_pContext->ClearRenderTargetView(m_pTargetView.Get(), color);
    m_pContext->ClearDepthStencilView(m_pDSView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}


void Graphics::EndFrame()
{
    if (m_imguiEnabled)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    HRESULT hr;
#ifndef NDEBUG
    m_infoManager.Set();
#endif

    // present Back Buffer on the screen
    if (FAILED(hr = m_pSwapChain->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
            throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
        else
            throw GFX_EXCEPT(hr);
    }
}

void Graphics::DrawIndexed(UINT count) NOXND
{
    GFX_THROW_INFO_ONLY(m_pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
    m_projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
    return m_projection;
}

