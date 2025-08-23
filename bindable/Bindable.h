#ifndef __BINDABLE_H
#define __BINDABLE_H

#include "Graphics.h"

class Bindable
{
public:
    virtual void Bind(Graphics &gfx) noexcept = 0;
    virtual ~Bindable() = default;

protected:
    static ID3D11DeviceContext *GetContext(Graphics &gfx) noexcept;
    static ID3D11Device        *GetDevice(Graphics &gfx) noexcept;
    static DxgiInfoManager     &GetInfoManager(Graphics &gfx) noexcept(!IS_DEBUG);
};

#endif //__BINDABLE_H
