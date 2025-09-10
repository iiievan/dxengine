#include "TransformCbuf.h"

#include "VertexBuffer.h"
TransformCbuf::TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot)
    : m_parent(parent)
{
    if (!m_pVcbuf)
        m_pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
}

void TransformCbuf::Bind(Graphics &gfx) noexcept
{
    const auto model = m_parent.GetTransformXM();

    const Transforms tf =
    {
        DirectX::XMMatrixTranspose(model),
        DirectX::XMMatrixTranspose(model * gfx.GetCamera() * gfx.GetProjection())
    };

    m_pVcbuf->Update(gfx,tf);
    m_pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_pVcbuf;