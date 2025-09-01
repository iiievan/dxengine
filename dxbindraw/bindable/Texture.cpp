#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacroses.h"

namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics &gfx, const class Surface &s)
{
    INFOMAN(gfx);

    // create texture resource
    D3D11_TEXTURE2D_DESC texdesc = {};
    texdesc.Width = s.GetWidth();
    texdesc.Height = s.GetHeight();
    texdesc.MipLevels = 1;
    texdesc.ArraySize = 1;
    texdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // same as back buffer
    texdesc.SampleDesc.Count = 1;
    texdesc.SampleDesc.Quality = 0;
    texdesc.Usage = D3D11_USAGE_DEFAULT;
    texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texdesc.CPUAccessFlags = 0;
    texdesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = s.GetBufferPtr();
    sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color); // pitch between 1 pixel of zero row and 1 pixel of the 1st row

    wrl::ComPtr<ID3D11Texture2D> pTexture;
    GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texdesc, &sd, &pTexture));

    // We dont bind texture itself to pipeline, we bind view of texture to pipeline
    // the same way it is done when initializing the depth buffer(CreateDepthStencilView in Graphics::Graphics(HWND hWnd))
    // there we create a dynamic depth texture wrl::ComPtr<ID3D11Texture2D> pDepthStencil and a view of that texture
    //  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd to bind the buffer to pipeline according to philosophy of DirectX
    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
    srvdesc.Format = texdesc.Format;
    srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvdesc.Texture2D.MostDetailedMip = 0;
    srvdesc.Texture2D.MipLevels = 1;
    GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvdesc,&m_pTextureView));
}

void Texture::Bind(Graphics &gfx) noexcept
{
    GetContext(gfx)->PSSetShaderResources(0u,1u,m_pTextureView.GetAddressOf());
}