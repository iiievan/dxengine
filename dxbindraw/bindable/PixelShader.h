#ifndef __PIXELSHADER_H
#define __PIXELSHADER_H

#include "Bindable.h"
#include "PixelShader.h"

namespace Bind
{
    class PixelShader : public Bindable
    {
    public:
        PixelShader(Graphics &gfx, const std::wstring &path);

        void Bind(Graphics &gfx) noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    };
}

#endif //__PIXELSHADER_H
