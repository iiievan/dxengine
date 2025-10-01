#ifndef __BINDABLECODEX_H
#define __BINDABLECODEX_H

#include "Bindable.h"
#include <memory>
#include <unordered_map>

namespace Bind
{
    class Codex
    {
    public:
        static std::shared_ptr<Bindable> Resolve(const std::string &key) NOXND  {  return Get().Resolve_(key);  }
        static void Store(std::shared_ptr<Bindable> bind) { Get().Store_(std::move(bind)); }

    private:
        std::shared_ptr<Bindable> Resolve_(const std::string &key) const NOXND
        {
            auto i = m_binds.find(key);
            if (i == m_binds.end())
                return {};

            return i->second;
        }

        void Store_(std::shared_ptr<Bindable> bind)
        {
            m_binds[bind->GetUID()] = std::move(bind);
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
