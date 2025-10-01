#ifndef __BINDABLECODEX_H
#define __BINDABLECODEX_H

#include "Bindable.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

namespace Bind
{
    class Codex
    {
    public:
        template<class T, typename... Params>
        static std::shared_ptr<Bindable> Resolve(Graphics& gfx, Params&&... p) NOXND
        {
            // Params&&... не обычные ссылки, а универсальные - могут принимать как lvalue, так и rvalue
            static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
            return Get().m_Resolve<T>(gfx,std::forward<Params>(p)...);
        }

    private:
        template<class T, typename... Params>
        std::shared_ptr<Bindable> m_Resolve(Graphics& gfx, Params&&... p)  NOXND
        {
            // std::forward - сохраняет категорию значения.
            // Если параметр был передан как lvalue - останется lvalue (Codex::Resolve<VertexBuffer>(gfx, std::move(vertexData));)
            // Если параметр был передан как rvalue - останется rvalue (Codex::Resolve<PixelShader>(gfx, "shader.hlsl");)
            const auto key = T::GenerateUID(std::forward<Params>(p)...);
            auto i = m_binds.find(key);
            if (i == m_binds.end())
            {
                auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
                m_binds[key] = bind;
                return bind;
            }

            return i->second;
        }

        static Codex& Get()
        {
            static Codex instance;
            return instance;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Bindable>> m_binds;
    };
}
#endif //__BINDABLECODEX_H
