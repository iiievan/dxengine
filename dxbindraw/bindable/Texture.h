#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "Bindable.h"

class Surface;

namespace Bind
{
    class Texture : public Bindable
    {
    public:
        Texture(Graphics &gfx, const  Surface &s, unsigned int slot = 0);
        void Bind(Graphics &gfx) noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;

    private:
        unsigned int m_slot;
    };
}

#endif //__TEXTURE_H
