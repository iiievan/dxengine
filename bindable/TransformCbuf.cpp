#include "TransformCbuf.h"
TransformCbuf::TransformCbuf(Graphics &gfx, const Drawable &parent)
    :m_vcbuf(gfx), m_parent(parent)
{

}

void TransformCbuf::Bind(Graphics &gfx) noexcept
{
    m_vcbuf.Update(gfx,DirectX::XMMatrixTranspose(m_parent.GetTransformXM() * gfx.GetProjection()));
    m_vcbuf.Bind(gfx);
}