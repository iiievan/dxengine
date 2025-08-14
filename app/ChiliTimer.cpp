#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer()
{
    m_last = steady_clock::now();
}

float ChiliTimer::Mark()
{
    const auto old = m_last;
    m_last = steady_clock::now();
    const duration<float> frameTime = m_last - old;
    return frameTime.count();
}

float ChiliTimer::Peek() const
{
    return duration<float>( steady_clock::now() - m_last).count();
}
