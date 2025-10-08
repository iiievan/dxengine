#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <d3d11.h>
#include "ChiliException.h"
#include <wrl.h>
#include <vector>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ConditionalNoexcept.h"
#include <dxgi.h>

namespace Bind
{
    class Bindable;
}

class Graphics
{
    friend class Bind::Bindable;
public:
    class Exception : public ChiliException
    {
        using ChiliException::ChiliException;
    };

    class HrException : public Exception
    {
    public:
        HrException(int line, const char *file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;

        const char *what() const noexcept override;
        const char *GetType() const noexcept override { return "Chili Graphics Exception"; }
        HRESULT     GetErrorCode() const noexcept     { return m_hr;  }
        std::string GetErrorString() const noexcept;
        std::string GetErrorDescription() const noexcept;
        std::string GetErrorInfo() const noexcept;

    private:
        HRESULT m_hr;
        std::string m_info;
    };

    class InfoException : public Exception
    {
    public:
        InfoException(int line, const char *file, std::vector<std::string> infoMsgs) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override { return "Chili Graphics Info Exception"; }
        std::string GetErrorInfo() const noexcept { return m_info; }
    private:
        std::string m_info;
    };

    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;

    public:
        const char *GetType() const noexcept override;
    private:
        std::string m_reason;
    };

public:
    Graphics(HWND hWnd, int width, int height);
    Graphics(const Graphics &graphics) = delete;
    Graphics &operator=(const Graphics &graphics) = delete;
    ~Graphics();

    void BeginFrame(float red, float green, float blue) noexcept;
    void EndFrame();
    void DrawIndexed( UINT count ) NOXND;
    void SetProjection( DirectX::FXMMATRIX proj ) noexcept;
    DirectX::XMMATRIX GetProjection() const noexcept;
    void SetCamera(DirectX::FXMMATRIX camera) noexcept { m_camera = camera;}
    DirectX::XMMATRIX GetCamera() const noexcept { return m_camera; }

    void EnableImgui() noexcept { m_imguiEnabled = true; }
    void DisableImgui() noexcept { m_imguiEnabled = false; }
    bool IsImguiEnabled() const noexcept { return m_imguiEnabled; }

    void LogAdapters();
    std::string GetActiveAdapterInfo() const;

private:
    // Adapter information
    Microsoft::WRL::ComPtr<IDXGIAdapter> SelectBestAdapter();

    Microsoft::WRL::ComPtr<IDXGIAdapter> m_pActiveAdapter;
    std::string m_adapterDescription;
    long long m_adapterMemory;

private:
    DirectX::XMMATRIX m_projection;
    DirectX::XMMATRIX m_camera;

#ifndef NDEBUG
    DxgiInfoManager m_infoManager;
#endif

    Microsoft::WRL::ComPtr<ID3D11Device>           m_pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         m_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSView;

    bool m_imguiEnabled {true};
};

#endif //__GRAPHICS_H
