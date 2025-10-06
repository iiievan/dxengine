#ifndef __TRANSFORMCBUFDOUBLE_H
#define __TRANSFORMCBUFDOUBLE_H

#include "TransformCbuf.h"

namespace Bind
{
    class TransformCbufDouble : public TransformCbuf
    {
    public:
        /* Creates a constant transform buffer in both the vertex and pixel shaders.
         * @param 'parent' - Drawable parent link adress(usually this pointer)
         * @param  'slotV' - Vertex shader slot num for constant transform buffer(usually 0)
         * @param  'slotP' - Pixel shader slot num for constant transform buffer(usually 2)
         */
        TransformCbufDouble(Graphics &gfx, const Drawable &parent, UINT slotV = 0u, UINT slotP = 0u);
        void Bind(Graphics &gfx) noexcept override;

    protected:
        void UpdateBindImpl(Graphics &gfx, const Transforms &tf) noexcept;

    private:
        static std::unique_ptr<PixelConstantBuffer<Transforms>> m_pPCbuf;

    };
}




#endif //__TRANSFORMCBUFDOUBLE_H
