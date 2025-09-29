#include "Drawable.h"
#include <cassert>
#include "GraphicsThrowMacroses.h"
#include "bindable/IndexBuffer.h"

using namespace Bind;

void Drawable::Draw(Graphics &gfx) const noexcept(!true)
{
    for (auto& b : m_binds)
        b->Bind(gfx);

    gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept(!true)
{
    // special case for index buffer
    if (typeid(*bind) == typeid(IndexBuffer))
    {
        assert("Binding multiple index buffers not allowed" && m_pIndexBuffer == nullptr);
        m_pIndexBuffer = &static_cast<IndexBuffer &>(*bind);
    }

    m_binds.push_back(std::move(bind));
}
