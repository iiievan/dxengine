#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char key_code) const noexcept
{
    return m_KeyStates[key_code];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
    if (m_KeyBuffer.size() > 0u)
    {
        Event e = m_KeyBuffer.front();
        m_KeyBuffer.pop();
        return e;
    }

    return { };
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return m_KeyBuffer.empty();
}

std::optional<char> Keyboard::ReadChar() noexcept
{
    if (m_charBuffer.size() > 0u)
    {
        unsigned char charcode = m_charBuffer.front();
        m_charBuffer.pop();
        return charcode;
    }

    return { };
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return m_charBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
    m_KeyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
    m_charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
    m_autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
    m_autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
    return m_autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char key_code) noexcept
{
    m_KeyStates[key_code] = true;
    m_KeyBuffer.push(Event(Event::Type::PRESS,key_code));
    TrimBuffer(m_KeyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char key_code) noexcept
{
    m_KeyStates[key_code] = false;
    m_KeyBuffer.push(Event(Event::Type::RELEASE,key_code));
    TrimBuffer(m_KeyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
    m_charBuffer.push(character);
    TrimBuffer(m_charBuffer);
}

void Keyboard::ClearState() noexcept
{
    m_KeyStates.reset();
}

template <typename T>
void Keyboard::TrimBuffer(std::queue<T> &buffer) noexcept
{
    while (buffer.size() > m_BuffSize)
    {
        buffer.pop();
    }
}


