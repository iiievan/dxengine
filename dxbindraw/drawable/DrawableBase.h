#ifndef __DRAWABLEBASE_H
#define __DRAWABLEBASE_H
#include "Drawable.h"
#include "bindable/IndexBuffer.h"

template <class T>
class DrawableBase : public Drawable
{
protected:
    bool IsStaticInitialized() const noexcept { return !m_staticBinds.empty(); }

    void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
    {
        assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
        m_staticBinds.push_back(std::move(bind));
    }

    void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
    {
        assert(m_pIndexBuffer == nullptr);
        m_pIndexBuffer = ibuf.get();
        m_staticBinds.push_back(std::move(ibuf));
    }

    void SetIndexFromStatic() noexcept(!IS_DEBUG)
    {
        assert( "Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr );
        for( const auto& b : m_staticBinds )
        {
            if( const auto p = dynamic_cast<IndexBuffer*>(b.get()) )
            {
                m_pIndexBuffer = p;
                return;
            }
        }
        assert( "Failed to find index buffer in static binds" && m_pIndexBuffer != nullptr );
    }

private:
    const std::vector<std::unique_ptr<Bindable>> &GetStaticBinds() const noexcept override { return m_staticBinds; }
    static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};

template <class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;

#endif //__DRAWABLEBASE_H
