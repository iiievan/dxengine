#ifndef __VERTEX_H
#define __VERTEX_H
#include <DirectXMath.h>
#include <type_traits>
#include <vector>

namespace hw3dexp
{
    struct BGRAColor
    {
        unsigned char a;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    class VertexLayout
    {
    public:
        enum ElementType
        {
            Position2D,
            Position3D,
            Texture2D,
            Normal,
            Float3Color,
            Float4Color,
            BGRAColor
        };

        class Element
        {
        public:
            Element(ElementType type, size_t offset) : m_type(type), m_offset(offset) {}

            static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
            {
                using namespace DirectX;
                switch (type)
                {
                    case Position2D:
                        return sizeof(XMFLOAT2);
                    case Position3D:
                        return sizeof(XMFLOAT3);
                    case Texture2D:
                        return sizeof(XMFLOAT2);
                    case Normal:
                        return sizeof(XMFLOAT3);
                    case Float3Color:
                        return sizeof(XMFLOAT3);
                    case Float4Color:
                        return sizeof(XMFLOAT3);
                    case BGRAColor:
                        return sizeof(hw3dexp::BGRAColor);
                }
                assert("Invalid element type" && false);
                return 0u;
            }
            size_t      Size() const noexcept(!IS_DEBUG) { return SizeOf(m_type); }
            size_t      GetOffsetAfter() const noexcept(!IS_DEBUG) { return m_offset + Size(); }
            size_t      GetOffset() const { return m_offset; }
            ElementType GetType() const noexcept { return m_type; }

        private:
            ElementType m_type;
            size_t      m_offset;
        };

    public:
        template <ElementType Type>
        const Element &Resolve() const noexcept(!IS_DEBUG)
        {
            for (auto &e : m_elements)
            {
                if (e.GetType() == Type)
                    return e;
            }
            assert("Could not resolve element type" && false);
            return m_elements.front();
        }

        const Element &ResolveByIndex(size_t i) const noexcept(!IS_DEBUG) { return m_elements[i]; }
        size_t Size() const noexcept(!IS_DEBUG) { return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter(); }
        size_t GetElementCount() const noexcept { return m_elements.size(); }

        template <ElementType Type>
        VertexLayout &Append() noexcept(!IS_DEBUG)
        {
            m_elements.emplace_back(Type, Size());
            return *this;
        }

    private:
        std::vector<Element> m_elements;
    };

    class Vertex
    {
        friend class VertexBuffer;

    public:
        template <VertexLayout::ElementType Type>
        auto &Attr() noexcept(!IS_DEBUG)
        {
            using namespace DirectX;

            const auto &element = m_layout.Resolve<Type>();
            auto        pAttribute = m_pData + element.GetOffset();

            // if constexpr - метапрограммирование! Компилятор выбирает только одну ветку на этапе компиляции в зависимости
            // от Type
            if constexpr (Type == VertexLayout::Position2D)
                return *reinterpret_cast<XMFLOAT2 *>(pAttribute);
            else if constexpr (Type == VertexLayout::Position3D)
                return *reinterpret_cast<XMFLOAT3 *>(pAttribute);
            else if constexpr (Type == VertexLayout::Texture2D)
                return *reinterpret_cast<XMFLOAT2 *>(pAttribute);
            else if constexpr (Type == VertexLayout::Normal)
                return *reinterpret_cast<XMFLOAT3 *>(pAttribute);
            else if constexpr (Type == VertexLayout::Float3Color)
                return *reinterpret_cast<XMFLOAT3 *>(pAttribute);
            else if constexpr (Type == VertexLayout::Float4Color)
                return *reinterpret_cast<XMFLOAT4 *>(pAttribute);
            else if constexpr (Type == VertexLayout::BGRAColor)
                return *reinterpret_cast<BGRAColor *>(pAttribute);
            else
            {
                assert("Bad element type" && false);
                return *reinterpret_cast<char *>(pAttribute);
            }
        }

        template <typename T>
        void SetAttributeByIndex(size_t i, T &&val) noexcept(IS_DEBUG)
        {
            using namespace DirectX;
            const auto &element = m_layout.ResolveByIndex(i);
            auto        pAttribute = m_pData + element.GetOffset();
            switch (element.GetType())
            {
                case VertexLayout::Position2D:
                    SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Position3D:
                    SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Texture2D:
                    SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Normal:
                    SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Float3Color:
                    SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Float4Color:
                    SetAttribute<XMFLOAT4>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::BGRAColor:
                    SetAttribute<BGRAColor>(pAttribute, std::forward<T>(val));
                    break;
                default:
                    assert("Bad element type" && false);
            }
        }

    protected:
        Vertex(char *pData, const VertexLayout &layout) noexcept(!IS_DEBUG) : m_pData(pData), m_layout(layout)
        {
            assert(pData != nullptr);
        }

    private:
        // enables parameter pack setting of multiple parameters by element index( more than one!!!)
        template <typename First, typename... Rest>
        void SetAttributeByIndex(size_t i, First &&first, Rest &&...rest) noexcept(!IS_DEBUG)
        {
            SetAttributeByIndex(i, std::forward<First>(first));
            SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
        }

        // helper to reduce code duplication in SetAttributeByIndex
        template <typename Dest, typename Src>
        void SetAttribute(char *pAttribute, Src &&val) noexcept(!IS_DEBUG)
        {
            if constexpr (std::is_assignable<Dest, Src>::value)
                *reinterpret_cast<Dest *>(pAttribute) = val;
            else
                assert("Parameter attribute type mismatch" && false);
        }

    private:
        char               *m_pData = nullptr;
        const VertexLayout &m_layout;
    };

    class ConstVertex
    {
    public:
        ConstVertex(const Vertex &v) noexcept(IS_DEBUG) : m_vertex(v) {}

        template <VertexLayout::ElementType Type>
        const auto &Attr() const noexcept(IS_DEBUG)
        {
            return const_cast<Vertex &>(m_vertex).Attr<Type>();
        }

    private:
        Vertex m_vertex;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG) : m_layout(std::move(layout)) {}

        const VertexLayout &GetLayout() const noexcept { return m_layout; }
        const char* GetGata() const noexcept(!IS_DEBUG) { return m_buffer.data(); }

        // return this size IS NOT of bytes, but returns size in vertices num.
        size_t Size() const noexcept(!IS_DEBUG) { return m_buffer.size() / m_layout.Size(); }
        size_t SizeBytes() const noexcept(!IS_DEBUG) { return m_buffer.size(); }

        template <typename... Params>
        void EmplaceBack(Params &&...params) noexcept(!IS_DEBUG)
        {
            assert(
                sizeof...(params) == m_layout.GetElementCount() && "Param count doesn't match number of vertex elements");
            m_buffer.resize(m_buffer.size() + m_layout.Size());
            Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
        }

        Vertex Back() noexcept(!IS_DEBUG)
        {
            assert(m_buffer.size() != 0u);
            return Vertex {m_buffer.data() + m_buffer.size() - m_layout.Size(), m_layout};
        }

        Vertex Front() noexcept(!IS_DEBUG)
        {
            assert(m_buffer.size() != 0u);
            return Vertex {m_buffer.data(), m_layout};
        }

        Vertex operator[](size_t i) noexcept(!IS_DEBUG)
        {
            assert(i < Size());
            return Vertex {m_buffer.data() + m_layout.Size() * i, m_layout};
        }

        ConstVertex Back() const noexcept(!IS_DEBUG) { return const_cast<VertexBuffer *>(this)->Back(); }
        ConstVertex Front() const noexcept(!IS_DEBUG) { return const_cast<VertexBuffer *>(this)->Front(); }
        ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG) { return const_cast<VertexBuffer &>(*this)[i]; }

    private:
        std::vector<char> m_buffer;
        VertexLayout      m_layout;
    };
} // namespace hw3dexp

#endif //__VERTEX_H
