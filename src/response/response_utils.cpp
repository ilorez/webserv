#include "../../includes/container.hpp"

// Canonical Form
Response::Response()
    : _file_fd(-1)
{}

Response::Response(const Response &other)
    : _req(other._req), 
      _file_fd(other._file_fd)
{}

Response &Response::operator=(const Response &other)
{
  if (this != &other)
  {
    this->_req = other._req;
    this->_file_fd = -1;
  }
  return (*this);
}

Response::~Response() 
{}

// Helper function
void Response::initStatusCodes(std::map<int, std::string> &m)
{
    static const std::pair<int, const char*> codes[] = 
    {
        std::make_pair(100, " Continue"),
        std::make_pair(101, " Switching Protocols"),
        std::make_pair(200, " OK"),

        std::make_pair(201, " Created"),
        std::make_pair(202, " Accepted"),
        std::make_pair(204, " No Content"),
        
        std::make_pair(301, " Moved Permanently"),
        std::make_pair(302, " Found"),
        std::make_pair(303, " See Other"),
        std::make_pair(307, " Temporary Redirect"),
        std::make_pair(308, " Permanent Redirect"),
        
        std::make_pair(400, " Bad Request"),
        std::make_pair(401, " Unauthorized"),
        std::make_pair(403, " Forbidden"),
        std::make_pair(404, " Not Found"),
        std::make_pair(405, " Method Not Allowed"),
        std::make_pair(408, " Request Timeout"),
        std::make_pair(409, " Conflict"),
        std::make_pair(411, " Length Required"),
        std::make_pair(413, " Payload Too Large"),
        std::make_pair(414, " URI Too Long"),
        std::make_pair(415, " Unsupported Media Type"),
        std::make_pair(429, " Too Many Requests"),
        std::make_pair(431, " Request Header Fields Too Large"),
        
        std::make_pair(500, " Internal Server Error"),
        std::make_pair(501, " Not Implemented"),
        std::make_pair(502, " Bad Gateway"),
        std::make_pair(503, " Service Unavailable"),
        std::make_pair(504, " Gateway Timeout"),
        std::make_pair(505, " HTTP Version Not Supported")
    };

    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); ++i)
        m.insert(codes[i]);
}

void Response::initMediaTypes(std::map<std::string, std::string> &m)
{
    static const std::pair<const char*, const char*> types[] =
    {
        // Text
        std::make_pair(".txt",  "text/plain"),
        std::make_pair(".html", "text/html"),
        std::make_pair(".css",  "text/css"),
        std::make_pair(".csv",  "text/csv"),
        std::make_pair(".py",  "text/x-python"),
        std::make_pair(".js",   "application/javascript"),
        std::make_pair(".php",   "application/x-httpd-php"),
        // Application
        std::make_pair(".json", "application/json"),
        std::make_pair(".xml",  "application/xml"),
        std::make_pair(".pdf",  "application/pdf"),
        std::make_pair(".zip",  "application/zip"),

        // Images
        std::make_pair(".png",  "image/png"),
        std::make_pair(".jpg",  "image/jpeg"),
        std::make_pair(".jpeg", "image/jpeg"),
        std::make_pair(".gif",  "image/gif"),
        std::make_pair(".bmp",  "image/bmp"),
        std::make_pair(".ico",  "image/x-icon"),
        std::make_pair(".svg",  "image/svg+xml"),
        std::make_pair(".webp", "image/webp"),

        // Audio
        std::make_pair(".mp3",  "audio/mpeg"),
        std::make_pair(".wav",  "audio/wav"),

        // Video
        std::make_pair(".mp4",  "video/mp4"),
        std::make_pair(".webm", "video/webm"),
        std::make_pair(".avi",  "video/x-msvideo")
    };

    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); ++i)
        m.insert(std::make_pair(types[i].first, types[i].second));
}

std::string Response::generateAutoIndex(const std::string& fullPath)
{
    DIR* dir = opendir(fullPath.c_str());
    if (!dir)
        return "";

    std::string body;
    body += "<html><body><h1>Index of " + _req.getPath() + "</h1><br>";

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // skip . and ..
        if (name == "." || name == "..")
            continue;

        std::string displayName = name;
        std::string href = _req.getPath();
        if (href[href.size() - 1] != '/')
            href += "/";
        href += name;

        struct stat st;
        std::string fullEntryPath = fullPath + "/" + name;

        if (stat(fullEntryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
            displayName += "/";

        body += "<a href=\"" + href + "\">" + displayName + "</a><br>";
    }

    closedir(dir);

    body += "</body></html>";
    return body;
}

bool    Response::isMethodAllowed(const std::string& method)
{
    if (!_loc)
        return true;

    const std::vector<std::string>& methods = _loc->getMethods();

    if (methods.empty())
        return true;

    if (std::find(methods.begin(), methods.end(), method) == methods.end())
    {
        serveErrorPage(405);
        return false;
    }
    return true;
}

bool    Response::tryApplyLocationReturn()
{
    if (!_loc)
        return false;

    if (!_loc->hasReturn())
        return false;

    _status = _loc->getReturnCode();
    _body   = _loc->getReturnUrl();
    return true;
}

std::string Response::returnFileExtension(const std::string& path)
{
    size_t pos = path.rfind('.');
    if (pos == std::string::npos)
        return "";
    
    size_t slash = path.rfind('/');
    if (slash != std::string::npos && slash > pos)
        return "";
    
    return path.substr(pos);
}

std::string Response::returnMediaType(const std::string& path)
{
    std::string ext = returnFileExtension(path);
    if (!ext.empty())
    {
        std::map<std::string, std::string>::iterator it = _mapMediaTypes.find(ext);
        if (it != _mapMediaTypes.end())
            return it->second;
    }
    return "text/html";
}

std::string Response::getExtensionFromContentType(
    const std::string &contentType,
    const std::map<std::string, std::string> &mediaTypes)
{
    for (std::map<std::string, std::string>::const_iterator it = mediaTypes.begin();
         it != mediaTypes.end(); ++it)
    {
        if (it->second == contentType)
            return it->first;
    }

    return "";
}

std::string Response::getHttpDate(time_t t)
{
    tm *gmt = gmtime(&t);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    return std::string(buffer);
}

std::string Response::generateUploadFileName(std::string contentTypeStr)
{
    std::string ret;
    std::srand(std::time(NULL));

    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    const char numset[]  = "0123456789";
    
    std::string random_part = "";

    for (int i = 0; i < 3; ++i)
    {
        random_part += charset[std::rand() % (sizeof(charset) - 1)];
    }

    for (int i = 0; i < 3; ++i)
    {
        random_part += numset[std::rand() % (sizeof(numset) - 1)];
    }

    ret += "client_body_" + random_part;
    ret += getExtensionFromContentType(contentTypeStr, _mapMediaTypes);
    
    return ret;
}

bool    Response::transferToNewFile(int destFd, int srcFd)
{
    char buffer[4096];
    ssize_t bytesRead;
    
    while ((bytesRead = read(srcFd, buffer, sizeof(buffer))) > 0) 
        if (write(destFd, buffer, bytesRead) == -1)
            return false;
    return true;
}

std::string Response::mergeResponseToString()
{
    std::string ret;

    ret += getHeaders();
    ret += "\r\n";
    ret += getBody();

    return (ret);
}

bool Response::isSupportedContentType(
    const std::string&                        contentType,
    const std::map<std::string, std::string>& mediaTypes)
{
    for (std::map<std::string, std::string>::const_iterator it = mediaTypes.begin();
         it != mediaTypes.end(); ++it)
    {
        if (it->second == contentType)
            return true;
    }
    return false;
}

// Getters
std::string Response::getBody()
{
    return (_body);
}

int         Response::getBodyFd()
{
    return (_file_fd);
}

std::string Response::getHeaders()
{
    std::string ret;
    std::map<int, std::string>::iterator sc_it = _mapStatusCodes.find(_status);
    ret = _req.getVersion() + " " + to_string98(sc_it->first) + " " + sc_it->second + "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
        it != _headers.end(); ++it) 
    {
        ret += it->first + ": " + it->second + "\r\n";
    }
    return (ret);
}

// Setters
void    Response::setReq(Request &req)
{
    this->_req = req;
}
