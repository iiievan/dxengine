#ifndef __DRAWABLE_H
#define __DRAWABLE_H

#include <DirectXMath.h>
#include "Graphics.h"
#include <memory>

class Bindable;

class Drawable
{
public:
    Drawable() = default;
    Drawable(const Drawable &) = delete;
    virtual ~Drawable() = default;

    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    virtual void              Update(float dt) noexcept = 0;

    void Draw(Graphics &gfx) const noexcept(!IS_DEBUG);
    void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
    void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;

private:
    const IndexBuffer                     *m_pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<Bindable>> m_binds;
};

#endif //__DRAWABLE_H
