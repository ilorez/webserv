#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

#define BUFFER_SIZE_LIMIT 65536

class Response
{
private:
    std::map<std::string, std::string>  _mapMediaTypes;
    std::map<int, std::string>          _mapStatusCodes;
    std::map<std::string, std::string>  _headers;
    std::string     _body;
    int             _status;
    Request         _req;
    int             _file_fd;

public:
    Response();
    ~Response();

    // setters
    void        setReq(Request &req);
    
    void        initStatusCodes(std::map<int, std::string> &m);
    void        initMediaTypes(std::map<std::string, std::string> &m);
    void        initHeaders(std::map<std::string, std::string> &h);
    
    std::string generateAutoIndex(const std::string& fullPath, const std::string& uriPath);
    std::string returnMediaType(const std::string& path);
    std::string mergeResponseToString();
    std::string build(int status);
    std::string build();

    std::string getHeaders();
    std::string getBody();
    int         getBodyFd();

    void        serveErrorPage(int status);
    void        Delete();
    void        Post();
    void        Get();

private:
    Response(const Response &other);
    Response &operator=(const Response &other);

};

#endif
