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
    int _status;
public:
    RequestException(const std::string& msg, int status);
    ~RequestException() throw();
    const char* what() const throw();
    int status() const throw();
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

class CGIException : public std::exception
{
private:
    std::string _msg;
    int _status;
public:
    CGIException(const std::string& msg, int status);
    ~CGIException() throw();
    const char* what() const throw();
    int status() const throw();
};




#endif
