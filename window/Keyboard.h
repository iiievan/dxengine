#ifndef __DXENGINE_KEYBOARD_H
#define __DXENGINE_KEYBOARD_H

#include <bitset>
#include <queue>
#include <optional>

class Keyboard
{
    friend class Window;

public:
    class Event
    {
    public:
        enum class Type
        {
            PRESS,
            RELEASE
        };

    private:
        Type          m_type;
        unsigned char m_code;

    public:
        Event(Type type, unsigned char code) : m_type(type), m_code(code) { }

        bool isPress() const noexcept { return m_type == Type::PRESS; }
        bool isRelease() const noexcept { return m_type == Type::RELEASE; }
        unsigned char getCode() const noexcept { return m_code; }
    };

public:
    Keyboard() = default;
    Keyboard(const Keyboard &) = delete;
    Keyboard &operator=(const Keyboard &) = delete;

    // key event stuff
    bool  KeyIsPressed(unsigned char key_code) const noexcept;
    std::optional<Event> ReadKey() noexcept;
    bool  KeyIsEmpty() const noexcept;

    // char event stuff
    std::optional<char> ReadChar() noexcept;
    bool CharIsEmpty() const noexcept;
    void FlushKey() noexcept;
    void FlushChar() noexcept;
    void Flush() noexcept;

    // autorepeat control
    void EnableAutorepeat() noexcept;
    void DisableAutorepeat() noexcept;
    bool AutorepeatIsEnabled() const noexcept;

private:
    void OnKeyPressed(unsigned char key_code) noexcept;
    void OnKeyReleased(unsigned char key_code) noexcept;
    void OnChar(char character) noexcept;
    void ClearState() noexcept;

    template <typename T>
    static void TrimBuffer(std::queue<T> &buffer) noexcept;

private:
    static constexpr unsigned int m_nKeys = 256u;
    static constexpr unsigned int m_BuffSize = 16u;
    bool                          m_autorepeatEnabled = false;
    std::bitset<m_nKeys>          m_KeyStates;
    std::queue<Event>             m_KeyBuffer;
    std::queue<char>              m_charBuffer;
};



#endif //__DXENGINE_KEYBOARD_H