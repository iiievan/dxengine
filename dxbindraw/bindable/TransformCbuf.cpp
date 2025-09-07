#include "TransformCbuf.h"

#include "VertexBuffer.h"
TransformCbuf::TransformCbuf(Graphics &gfx, const Drawable &parent)
    : m_parent(parent)
{
    if (!m_pVcbuf)
        m_pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
}

void TransformCbuf::Bind(Graphics &gfx) noexcept
{
    m_pVcbuf->Update(gfx,DirectX::XMMatrixTranspose(m_parent.GetTransformXM() *
                                                                gfx.GetCamera() *
                                                                gfx.GetProjection()));
    m_pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::m_pVcbuf;