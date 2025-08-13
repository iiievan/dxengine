#ifndef CHILIEXCEPTION_H
#define CHILIEXCEPTION_H

#include <exception>
#include <string>

class ChiliException : public std::exception
{
public:
    ChiliException(int line, const char *file) noexcept
    : m_line(line), m_file(file) {}

    virtual const char *GetType() const noexcept { return "Chili Exception"; }
    int                 GetLine() const noexcept { return m_line; }
    const std::string  &GetFile() const noexcept { return m_file; }

    const char         *what() const noexcept override;
    std::string         GetOriginString() const noexcept;

private:
    int         m_line;
    std::string m_file;

protected:
    mutable std::string m_whatBuffer;
};

#endif // CHILIEXCEPTION_H
