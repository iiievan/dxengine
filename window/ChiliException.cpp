#include "ChiliException.h"
#include <sstream>


const char *ChiliException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl << GetOriginString();

    m_whatBuffer = oss.str();

    return m_whatBuffer.c_str();
}

std::string ChiliException::GetOriginString() const noexcept
{
    std::ostringstream oss;

    oss << "[File]" << m_file << std::endl << "[Line]" << m_line;

    return oss.str();
}