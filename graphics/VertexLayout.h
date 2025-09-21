#ifndef __VERTEXLAYOUT_H
#define __VERTEXLAYOUT_H
#include <vector>
#include <DirectXMath.h>
#include <type_traits>

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
                    return sizeof(unsigned int);
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
    friend class VertexLayoutBuffer;

public:

    Vertex(char *pData, const VertexLayout &layout) noexcept(!IS_DEBUG)
    : m_pData(pData), m_layout(layout)
    {
        assert(pData != nullptr);
    }

    template <VertexLayout::ElementType Type>
    auto &Attr() noexcept(!IS_DEBUG)
    {
        using namespace DirectX;

        const auto &element = m_layout.Resolve<Type>();
        auto        pAttribute = m_pData + element.GetOffset();

        // if constexpr - метапрограммирование! Компилятор выбирает только одну ветку на этапе компиляции в зависимости от Type
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

class VertexLayoutBuffer
{
public:

    VertexLayoutBuffer(VertexLayout layout) noexcept(!IS_DEBUG) : m_layout(layout) {}

    const VertexLayout &GetLayout() const noexcept { return m_layout; }

    // return this size IS NOT of bytes, but returns size in vertices num.
    size_t              Size() const noexcept(!IS_DEBUG) { return m_buffer.size() / m_layout.Size(); }

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

private:
    std::vector<char> m_buffer;
    VertexLayout      m_layout;
};

#endif //__VERTEXLAYOUT_H
