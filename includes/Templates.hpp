#ifndef TEMPLATES_HPP
#define TEMPLATES_HPP

#include <sstream>
#include <string>

template <typename T>
std::string to_string98(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename T, typename R>
bool to_integer(T value, R &re)
{
    std::istringstream iss(value);
    return (iss >> re);
}

#endif
