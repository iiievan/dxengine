#include "Drawable.h"
#include <cassert>
#include "GraphicsThrowMacroses.h"
#include "bindable/IndexBuffer.h"

void Drawable::Draw(Graphics &gfx) const noexcept(!true)
{
    for (auto& b : m_binds)
    {
        b->Bind(gfx);
    }
    for (auto& b : GetStaticBinds())
    {
        b->Bind(gfx);
    }

    gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!true)
{
    assert( "*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer) );
    m_binds.push_back( std::move( bind ) );
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG)
{
    assert( "Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr );
    m_pIndexBuffer = ibuf.get();
    m_binds.push_back( std::move( ibuf ) );
}