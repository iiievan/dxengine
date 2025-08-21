#ifndef __DXENGINE_MOUSE_H
#define __DXENGINE_MOUSE_H

#include <queue>
#include <optional>

class Mouse
{
    friend class Window;

public:
    class Event
    {
    public:
        enum class Type
        {
            L_PRESS,
            L_RELEASE,
            R_PRESS,
            R_RELEASE,
            WHEEL_UP,
            WHEEL_DOWN,
            MOVE,
            ENTER,      // cursor enter window area
            LEAVE       // cursor leave window area
        };

    private:
        Type m_type;
        bool m_left_pressed;
        bool m_right_pressed;
        int  m_x;
        int  m_y;

    public:
        Event(Type type, const Mouse& parent) noexcept
        : m_type(type),
        m_left_pressed(parent.m_leftIsPressed),
        m_right_pressed(parent.m_rightIsPressed),
        m_x(parent.m_x),
        m_y(parent.m_y)
        {}

        Type GetType() const noexcept { return m_type; }
        std::pair<int, int> GetPos() const noexcept { return {m_x, m_y}; }
        int GetX() const noexcept { return m_x; }
        int GetY() const noexcept { return m_y; }
        bool LeftIsPressed() const noexcept { return m_left_pressed; }
        bool RightIsPressed() const noexcept { return m_right_pressed; }
    };

public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    std::pair<int, int> GetPos() const noexcept { return {m_x, m_y}; }
    int GetX() const noexcept { return m_x; }
    int GetY() const noexcept { return m_y; }
    bool IsInWindow() const noexcept { return m_is_in_window; }
    bool LeftIsPressed() const noexcept { return m_leftIsPressed; }
    bool RightIsPressed() const noexcept { return m_rightIsPressed; }
    std::optional<Mouse::Event> Read() noexcept;
    bool IsEmpty() const noexcept { return m_buffer.empty(); }
    void Flush() noexcept;

private:
    void OnMouseMove(int , int ) noexcept;
    void OnMouseLeave() noexcept;
    void OnMouseEnter() noexcept;
    void OnLeftPressed(int , int ) noexcept;
    void OnLeftReleased(int , int ) noexcept;
    void OnRightPressed(int , int ) noexcept;
    void OnRightReleased(int , int ) noexcept;
    void OnWheelUp(int , int ) noexcept;
    void OnWheelDown(int , int ) noexcept;
    void OnWheelDelta(int,int,int ) noexcept;
    void TrimBuffer() noexcept;

private:
    static constexpr unsigned int m_BuffSize = 16u;
    int m_x;
    int m_y;
    bool m_leftIsPressed = false;
    bool m_rightIsPressed = false;
    bool m_is_in_window = false;
    int m_wheel_delta_carry = 0;
    std::queue<Event> m_buffer;
};


#endif //__DXENGINE_MOUSE_H