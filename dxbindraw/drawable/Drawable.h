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
public:
    Drawable() = default;
    Drawable(const Drawable &) = delete;
    virtual ~Drawable() = default;

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    void Draw(Graphics &gfx) const NOXND;

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

protected:
    void AddBind(std::shared_ptr<Bind::Bindable> bind) NOXND;

private:
    const Bind::IndexBuffer                     *m_pIndexBuffer = nullptr;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;
};

#endif //__DRAWABLE_H
