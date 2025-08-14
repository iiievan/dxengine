#include "Mouse.h"

Mouse::Event Mouse::Read() noexcept
{
    if( m_buffer.size() > 0u )
    {
        Event e = m_buffer.front();
        m_buffer.pop();
        return e;
    }

    return Event();
}

void Mouse::Flush() noexcept
{
    m_buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
    m_x = newx;
    m_y = newy;

    m_buffer.push(Event(Event::Type::MOVE,*this));
    TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
    m_leftIsPressed = true;

    m_buffer.push(Event(Event::Type::L_PRESS,*this));
    TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
    m_leftIsPressed = false;

    m_buffer.push(Event(Event::Type::L_RELEASE,*this));
    TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
    m_rightIsPressed = true;

    m_buffer.push(Event(Event::Type::R_PRESS,*this));
    TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
    m_rightIsPressed = false;

    m_buffer.push(Event(Event::Type::R_RELEASE,*this));
    TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
    m_buffer.push(Event(Event::Type::WHEEL_UP,*this));
    TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
    m_buffer.push(Event(Event::Type::WHEEL_DOWN,*this));
    TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
    while( m_buffer.size() > m_BuffSize )
    {
        m_buffer.pop();
    }
}