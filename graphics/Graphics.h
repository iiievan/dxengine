#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <d3d11.h>
#include "ChiliException.h"
#include <wrl.h>
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
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
    Graphics(HWND hWnd);
    Graphics(const Graphics &graphics) = delete;
    Graphics &operator=(const Graphics &graphics) = delete;
    ~Graphics() = default;
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;
    void DrawTestTriangle();


#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif

private:
    Microsoft::WRL::ComPtr<ID3D11Device>           m_pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         m_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTargetView;
};

#endif //__GRAPHICS_H
