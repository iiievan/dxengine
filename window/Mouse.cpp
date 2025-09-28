#include "Mouse.h"
#include "Utils.hpp"
#include "WinDefs.h"



std::optional<Mouse::Event> Mouse::Read() noexcept
{
    if( m_buffer.size() > 0u )
    {
        Event e = m_buffer.front();
        m_buffer.pop();
        return e;
    }

    return { };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
    if (m_rawDeltaBuffer.empty())
        return std::nullopt;

    const RawDelta d = m_rawDeltaBuffer.front();
    m_rawDeltaBuffer.pop();
    return d;
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

void Mouse::OnMouseLeave() noexcept
{
    m_is_in_window = false;
    m_buffer.push(Event(Event::Type::LEAVE,*this));
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    m_is_in_window = true;
    m_buffer.push(Event(Event::Type::ENTER,*this));
    TrimBuffer();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
    m_rawDeltaBuffer.push({dx,dy });
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
    m_buffer.push(Event(Event::Type::WHEEL_DOWN, *this));
    TrimBuffer();
}

void Mouse::TrimRawInputBuffer() noexcept
{
    while (m_rawDeltaBuffer.size() > m_BuffSize)
    {
        m_rawDeltaBuffer.pop();
    }
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
    m_wheel_delta_carry += delta;
    // generate events for every 120 tics
    while (m_wheel_delta_carry >= WHEEL_DELTA)
    {
        m_wheel_delta_carry -= WHEEL_DELTA;
        OnWheelUp(x,y);
    }

    while (m_wheel_delta_carry <= -WHEEL_DELTA)
    {
        m_wheel_delta_carry += WHEEL_DELTA;
        OnWheelDown(x,y);
    }
}

void Mouse::TrimBuffer() noexcept
{
    while( m_buffer.size() > m_BuffSize )
    {
        m_buffer.pop();
    }
}