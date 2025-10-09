#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "Bindable.h"

class Surface;

namespace Bind
{
    /* for DirectX UV(TB) coordinates directions like Windows Window coordinates:
    (0,0) U+→ (1,0)     |   (0,0) T+→ (1,0)
      ↓                 |     ↓
      V+                |     B+
      ↓                 |     ↓
    (0,1)     (1,1)     |   (0,1)     (1,1)
    */
    /* for OpenGL UV(TB) coordinates directions like Math Graphics:
    (0,1)     (1,1)     |  (0,1)     (1,1)
      ↑                 |   ↑
      V+                |   B+
      ↑                 |   ↑
    (0,0) U+→ (1,0)     |   (0,0) T+→ (1,0)
    */
    class Texture : public Bindable
    {
    public:
        Texture(Graphics &gfx, const std::string &path, UINT slot = 0);
        void Bind(Graphics &gfx) noexcept override;
        static std::shared_ptr<Texture> Resolve(Graphics &gfx, const std::string &path, UINT slot = 0);
        static std::string GenerateUID(const std::string &path, UINT slot = 0);
        std::string GetUID() const noexcept override;
        bool HasAlpha() const noexcept;

    protected:
        bool hasAlpha = false;
        std::string path;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;

    private:
        unsigned int m_slot;
    };
}

#endif //__TEXTURE_H
