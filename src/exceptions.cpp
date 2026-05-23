
#include "../includes/WebServExceptions.hpp"
    
// request
RequestException::RequestException(const std::string& msg, int status) : _msg(msg), _status(status) {}
const char* RequestException::what() const throw() { return _msg.c_str(); }
int RequestException::status() const throw() { return _status; }
RequestException::~RequestException() throw(){}

//server
ServerException::ServerException(const std::string& msg) : _msg(msg) {}
ServerException::~ServerException() throw() {}
const char* ServerException::what() const throw() { return _msg.c_str(); }

// response
ResponseException::ResponseException(const std::string& msg) : _msg(msg) {}
ResponseException::~ResponseException() throw() {}
const char* ResponseException::what() const throw() { return _msg.c_str(); }

// cgi
CGIException::CGIException(const std::string& msg, int status) : _msg(msg), _status(status) {}
CGIException::~CGIException() throw() {}
const char* CGIException::what() const throw() { return _msg.c_str(); }
int CGIException::status() const throw() { return _status; }




