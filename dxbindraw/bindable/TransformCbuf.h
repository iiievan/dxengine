#ifndef __TRANSFORMCBUF_H
#define __TRANSFORMCBUF_H

#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "drawable/Drawable.h"

class TransformCbuf : public Bindable
{
public:
    TransformCbuf(Graphics &gfx, const Drawable &parent);

    void Bind(Graphics &gfx) noexcept override;

private:
    static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> m_pVcbuf;
    const Drawable                         &m_parent;
};

#endif //__TRANSFORMCBUF_H
