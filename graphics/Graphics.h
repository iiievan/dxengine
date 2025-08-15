#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <d3d11.h>
#include "WinDefs.h"

class Graphics
{
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics &graphics) = delete;
    Graphics &operator=(const Graphics &graphics) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept
    {
        const float color[] = { red, green, blue, 1.0f };
        m_pContext->ClearRenderTargetView(m_pTarget,color);
    }

private:
    ID3D11Device           *m_pDevice {nullptr};
    IDXGISwapChain         *m_pSwapChain {nullptr};
    ID3D11DeviceContext    *m_pContext {nullptr};
    ID3D11RenderTargetView *m_pTarget {nullptr};
};

#endif //__GRAPHICS_H
