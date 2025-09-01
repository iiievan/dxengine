#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "Bindable.h"

class Texture : public Bindable
{
public:
    Texture(Graphics &gfx, const class Surface &s);
    void Bind(Graphics &gfx) noexcept override;

protected:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};



#endif //__TEXTURE_H
