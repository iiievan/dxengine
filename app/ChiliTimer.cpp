#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer() noexcept
{
    m_last = steady_clock::now();
}

float ChiliTimer::Mark() noexcept
{
    const auto old = m_last;
    m_last = steady_clock::now();
    const duration<float> frameTime = m_last - old;
    return frameTime.count();
}

float ChiliTimer::Peek() const noexcept
{
    return duration<float>( steady_clock::now() - m_last).count();
}
