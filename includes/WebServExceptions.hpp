#ifndef WEBSERVEXCEPTIONS_HPP
#define WEBSERVEXCEPTIONS_HPP

#include <exception>
#include <string>

class ServerException : public std::exception
{
private:
    std::string _msg;
public:
    ServerException(const std::string& msg);
    ~ServerException() throw();
    const char* what() const throw();
};

class RequestException : public std::exception
{
private:
    std::string _msg;
public:
    RequestException(const std::string& msg);
    ~RequestException() throw();
    const char* what() const throw();
};

class ResponseException : public std::exception
{
private:
    std::string _msg;
public:
    ResponseException(const std::string& msg);
    ~ResponseException() throw();
    const char* what() const throw();
};



#endif
