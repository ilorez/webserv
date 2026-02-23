
#include "../includes/WebServExceptions.hpp"
    
// request
RequestException::RequestException(const std::string& msg) : _msg(msg) {}
const char* RequestException::what() const throw() { return _msg.c_str(); }
RequestException::~RequestException() throw(){}

//server
ServerException::ServerException(const std::string& msg) : _msg(msg) {}
ServerException::~ServerException() throw() {}
const char* ServerException::what() const throw() { return _msg.c_str(); }

// response
ResponseException::ResponseException(const std::string& msg) : _msg(msg) {}
ResponseException::~ResponseException() throw() {}
const char* ResponseException::what() const throw() { return _msg.c_str(); }



