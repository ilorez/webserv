#ifndef TOSTRING_HPP
#define TOSTRING_HPP

#include <sstream>
#include <string>

template <typename T>
std::string to_string98(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif
