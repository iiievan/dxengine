#ifndef __DRAWABLE_H
#define __DRAWABLE_H

#include <DirectXMath.h>
#include <memory>
#include "ConditionalNoexcept.h"
#include "Graphics.h"

namespace Bind
{
    class Bindable;
    class IndexBuffer;
}

class Drawable
{
    template<class T>
friend class DrawableBase;
public:
    Drawable() = default;
    Drawable(const Drawable &) = delete;
    virtual ~Drawable() = default;

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    virtual void              Update(float dt) noexcept
    {

    }

    void Draw(Graphics &gfx) const NOXND;

protected:

    template<class T>
    T* QueryBindable() noexcept
    {
        for (auto& pb : m_binds)
        {
            if (auto pt = dynamic_cast<T*>(pb.get()))
                return pt;
        }
        return nullptr;
    }

    void AddBind(std::unique_ptr<Bind::Bindable> bind) NOXND;
    void AddIndexBuffer(std::unique_ptr<class Bind::IndexBuffer> ibuf) NOXND;

private:
    virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;
private:
    const Bind::IndexBuffer                     *m_pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<Bind::Bindable>> m_binds;
};

#endif //__DRAWABLE_H
