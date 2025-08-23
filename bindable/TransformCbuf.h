#ifndef __TRANSFORMCBUF_H
#define __TRANSFORMCBUF_H

#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "Drawable.h"

class TransformCbuf : public Bindable
{
public:
    TransformCbuf(Graphics &gfx, const Drawable &parent);

    void Bind(Graphics &gfx) noexcept override;

private:
    VertexConstantBuffer<DirectX::XMMATRIX> m_vcbuf;
    const Drawable                         &m_parent;
};

#endif //__TRANSFORMCBUF_H
