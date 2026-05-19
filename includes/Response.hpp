#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

#define BUFFER_SIZE_LIMIT 65536

namespace Default
{
    const std::string UPLOAD_STORE  = "./uploads";
    const std::string CGI_STORE  = "./cgi-bin/";
    const std::string ROOT          = "./www"; // alaoui:todo i don’t use that variable, so will delete it latter.
    const std::string ERROR_PAGE    = "./www/error/error.html";
    const std::string SERVER_NAME   = "MyServer";
}

class Response
{
private:
    std::map<std::string, std::string>  _mapMediaTypes;
    std::map<int, std::string>          _mapStatusCodes;
    std::map<std::string, std::string>  _headers;
    const LocationConfig*               _loc;
    std::string                         _body;
    Request                             _req;
    int                                 _status;
    int                                 _file_fd;

public:
    Response();
    ~Response();
    Response(const Response &other);
    Response &operator=(const Response &other);

    // setters
    void        setReq(Request &req);
    
    void        initStatusCodes(std::map<int, std::string> &m);
    void        initMediaTypes(std::map<std::string, std::string> &m);
    void        initHeaders(std::map<std::string, std::string> &h);
    
    std::string generateAutoIndex(const std::string& fullPath);
    std::string returnMediaType(const std::string& path);
    std::string mergeResponseToString();
    std::string build(int status);
    std::string build();
    std::string returnFileExtension(const std::string& path);
    std::string generateUploadFileName();
    std::string getHttpDate(time_t t);
    bool        isMethodAllowed(const std::string& method);
    bool        transferToNewFile(int destFd, int srcFd);
    bool        tryApplyLocationReturn();

    std::string getHeaders();
    std::string getBody();
    int         getBodyFd();

    void        serveErrorPage(int status);
    void        Delete();
    void        Post();
    void        Get();
};

#endif
