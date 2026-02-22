#ifndef WEBSERVEXCEPTIONS_HPP
#define WEBSERVEXCEPTIONS_HPP

#include <exception>
#include <string>

class ServerException : public std::exception
{
private:
    std::string _msg;
public:
    ServerException(const std::string& msg) : _msg(msg) {}
    const char* what() const throw() { return _msg.c_str(); }
};

class RequestException : public std::exception
{
private:
    std::string _msg;
public:
    RequestException(const std::string& msg) : _msg(msg) {}
    const char* what() const throw() { return _msg.c_str(); }
};

class ResponseException : public std::exception
{
private:
    std::string _msg;
public:
    ResponseException(const std::string& msg) : _msg(msg) {}
    const char* what() const throw() { return _msg.c_str(); }
};



#endif
