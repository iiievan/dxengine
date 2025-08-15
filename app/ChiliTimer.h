#ifndef __CHILITIMER_H
#define __CHILITIMER_H
#include <chrono>


class ChiliTimer
{
public:
    ChiliTimer() noexcept;
    float Mark() noexcept;
    float Peek() const noexcept;
private:
    std::chrono::steady_clock::time_point m_last;
};

#endif //__CHILITIMER_H
