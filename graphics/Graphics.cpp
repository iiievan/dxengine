#include "Graphics.h"
#include <d3dcompiler.h>
#include <sstream>
#include "Utils.hpp"
#include "dxerr.h"
#include <directxmath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) \
    if (FAILED(hr = (hrcall)))   \
    throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
    infoManager.Set();         \
    if (FAILED(hr = (hrcall))) \
    throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call)                                 \
    infoManager.Set();                                            \
    (call);                                                       \
    {                                                             \
        auto v = infoManager.GetMessages();                       \
        if (!v.empty())                                           \
        {                                                         \
            throw Graphics::InfoException(__LINE__, __FILE__, v); \
        }                                                         \
    }
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifndef NDEBUG
    infoManager.Set();
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

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = {red, green, blue, 1.0f};
    m_pContext->ClearRenderTargetView(m_pTargetView.Get(), color);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
    namespace wrl = Microsoft::WRL;
    HRESULT hr;

    /********************************************************* Create Vertex Buffer ***********************************************************************/
    struct Vertex
    {
        struct
        {
            float x;
            float y;
        } pos;

        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } color;
    };

    Vertex vertices[] =
    {
    /*0*/    {0.0f, 0.5f, 255, 0, 0, 0},
    /*1*/    {0.5f, -0.5f, 0, 255, 0, 0},
    /*2*/    {-0.5f, -0.5f, 0, 0, 255, 0},
    /*3*/    {-0.3f, 0.3f, 0, 255, 0, 0},
    /*4*/    {0.3f, 0.3f, 0, 0, 255, 0},
    /*5*/    {0.0f, -1.0f, 255, 0, 0, 0}
    };

    vertices[0].color.g = 255;

    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    GFX_THROW_INFO(m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
    m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

    /******************************************************** Create Index buffer ******************************************************************/
    const unsigned short indices[] =
    {
        0,1,2,
        0,2,3,
        0,4,1,
        2,1,5
    };

    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;

    GFX_THROW_INFO(m_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
    m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);


    /***************************************************** Create Index Buffer ***************************************************************/
    // create constant beffer for transformation matrix
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };

    const ConstantBuffer cb =
    {
        {
                dx::XMMatrixTranspose(dx::XMMatrixRotationZ(angle) *
                                         dx::XMMatrixScaling(3.0f/4.0f, 1.0f, 1.0f) *
                                         dx::XMMatrixTranslation(x, y, 0.0f))
        }
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd = {};
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    GFX_THROW_INFO(m_pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
    m_pContext->VSSetConstantBuffers(0u,1u, pConstantBuffer.GetAddressOf());

    /************************************************** Create Shaders *************************************************************************/
    // create pixel shader
    wrl::ComPtr<ID3DBlob>          pBlob;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    GFX_THROW_INFO(D3DReadFileToBlob(L"shaders/Triangle.ps.cso", &pBlob)); // set path in cmake-build-debug folder
    GFX_THROW_INFO(
        m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

    // bind pixel shader
    m_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    GFX_THROW_INFO(D3DReadFileToBlob(L"shaders/Triangle.vs.cso", &pBlob)); // set path in cmake-build-debug folder
    GFX_THROW_INFO(
        m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

    // bind vertex shader
    m_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);


    /****************************************************** Create Input Layout *********************************************************************/
    wrl::ComPtr<ID3D11InputLayout> pIL;
    const D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    GFX_THROW_INFO(m_pDevice->CreateInputLayout(
        ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pIL));

    // bind vertex layout
    m_pContext->IASetInputLayout(pIL.Get());


    /******************************************************** Rendering Process ********************************************************************/
    // set pixels of Back Buffer
    m_pContext->OMSetRenderTargets(1u, m_pTargetView.GetAddressOf(), nullptr);

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //  Настраивается Viewport — область назначения для отрисовки. Здесь мы говорим:
    //  "Рисуй на всю цель рендера (800x600), преобразовывая глубину из диапазона [0,1] в этот же диапазон".
    // Это преобразует координаты из NDC (-1 до 1) в координаты экрана (0 до 800, 0 до 600).
    D3D11_VIEWPORT vp = {};
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1u, &vp);

    GFX_THROW_INFO_ONLY(m_pContext->DrawIndexed((UINT)std::size(indices),0u, 0u));
}