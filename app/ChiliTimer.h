#ifndef __CHILITIMER_H
#define __CHILITIMER_H
#include <chrono>


class ChiliTimer
{
public:
    ChiliTimer();
    float Mark();
    float Peek() const;
private:
    std::chrono::steady_clock::time_point m_last;
};

#endif //__CHILITIMER_H
