#include "../includes/container.hpp"

Response::Response(Request &req)
  : _req(req)
{}

Response::Response(const Response &other) : _req(other._req)
{}

Response &Response::operator=(const Response &other)
{
  if (this != &other)
  {
    this->_req = other._req;
  }
  return (*this);
}

Response::~Response() {};

std::string getHttpDate(time_t t)
{
    tm *gmt = gmtime(&t);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    return std::string(buffer);
}

std::string returnFileExtension(const std::string& path)
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

void Response::initHeaders(std::map<std::string, std::string> &h)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // General headers
    h.insert(std::make_pair("Date", getHttpDate(ts.tv_sec)));
    h.insert(std::make_pair("Server", "MyServer"));
    h.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    h.insert(std::make_pair("Connection", "close"));

    // Content-Type
    if (_req.getMethod() == "GET")
        h.insert(std::make_pair("Content-Type", returnMediaType(_req.getPath())));
    else
        h.insert(std::make_pair("Content-Type", "text/plain"));

    if (_req.getMethod() == "GET")
    {
        struct stat _stat;
        std::string filepath;
        if (_status / 100 == 4 || _status / 100 == 5)
            filepath = "./www/errorPage.html";
        else
            filepath = "./www" + _req.getPath();
        if (stat(filepath.c_str(), &_stat) == -1)
            throw ResponseException("stat failure\n");
        h.insert(std::make_pair("Last-Modified", getHttpDate(_stat.st_mtim.tv_sec)));
    }
    else if (_req.getMethod() == "POST")
    {
        if (_status == 201)
            h.insert(std::make_pair("Location", _req.getPath()));
    }
    else if (_req.getMethod() == "DELETE")
    {
        
    }
}

std::string  Response::mergeResponseToString()
{
  std::string ret;
  std::map<int, std::string>::iterator sc_it = _mapStatusCodes.find(_status);

  // make start line
  ret += _req.getVersion() + " " + to_string98(sc_it->first) + " " + sc_it->second + "\r\n";
  
  // make headers
  for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
       it != _headers.end(); ++it) {
      ret += it->first + ": " + it->second + "\r\n";
  }

  // empty line 
  ret += "\r\n";

  // set the body
  ret += _body;

  return (ret);
}

void Response::serveErrorPage(int status)
{
    _status = status;

    std::string errorPage = "./www/errorPage.html";
    std::string codeStr = to_string98(status);
    std::string msgStr = "Unknown Error";
   
    _body = ft_readFile(errorPage);
    if (_body.empty())
    {
        _body = codeStr + " Error";
        return;
    }

    std::map<int, std::string>::iterator it = _mapStatusCodes.find(status);
    if (it != _mapStatusCodes.end())
        msgStr = it->second;

    size_t pos = 0;
    while ((pos = _body.find("{{CODE}}", pos)) != std::string::npos)
    {
        _body.replace(pos, 8, codeStr);
        pos += codeStr.length();
    }

    pos = 0;
    while ((pos = _body.find("{{MESSAGE}}", pos)) != std::string::npos)
    {
        _body.replace(pos, 11, msgStr);
        pos += msgStr.length();
    }
}

void Response::Get()
{
    /*
    // Check if POST is allowed on this route
    if ()
    {
        _status = 405;
        _body = "Method Not Allowed";
        return;
    }
    */

    std::string path = _req.getPath();

    std::string filepath;
    if (path == "/" || path == "/index.html")
        filepath = "./www/index.html";
    else
        filepath = "./www" + path;

    if (access(filepath.c_str(), F_OK) != 0)
    {
        serveErrorPage(404);
        return;
    }

    if (access(filepath.c_str(), R_OK) != 0)
    {
        serveErrorPage(403);
        return;
    }

    struct stat info;
    stat(filepath.c_str(), &info);
    if (S_ISDIR(info.st_mode))
    {
        filepath += "/index.html";
        if (access(filepath.c_str(), F_OK) != 0)
        {
            serveErrorPage(403);
            return;
        }
    }

    _body = ft_readFile(filepath);
    if (_body.empty())
    {
        serveErrorPage(404);
        return;
    }

    _status = 200;
}

void Response::Post()
{
    /*
    // Check if POST is allowed on this route
    if ()
    {
        _status = 405;
        _body = "Method Not Allowed";
        return;
    }
    */


    std::string contentType = _req.getHeaderValue("content-type");
    std::string contentLengthStr = _req.getHeaderValue("content-length");
    if (contentType.empty() || contentLengthStr.empty())
    {
        _status = 400;
        _body = "Missing Content-Type or Content-Length headers.\n";
        return;
    }

    if (contentType != "application/x-www-form-urlencoded")
    {
        _status = 415;
        _body = "Unsupported Content-Type\n";
        return;
    }

    int contentLength = 0;
    contentLength = std::atoi(contentLengthStr.c_str());

    if (contentLength <= 0)
    {
        _status = 411;
        _body = "Content-Length missing or zero\n";
        return;
    }

    std::string filepath = "./www" + _req.getPath();
    std::ofstream file(filepath.c_str());
    if (file)
    {
        /* Replace line two with one if the request stores the body.*/
        file << "Some text , should be body\n";
        // file << _req.getbody();
        _status = 201;
        _body = "File created successfully\n";
    }
    else
    {
        _status = 500;
        _body = "Internal Server Error: could not create file\n";
    }
}


void Response::Delete()
{
    /*
    // Check if DELETE is allowed on this route
    if ()
    {
        _status = 405;
        _body = "Method Not Allowed";
        return;
    }
    */

    std::string filepath = "./www" + _req.getPath();

    if (access(filepath.c_str(), F_OK) != 0)
    {
        _status = 404;
        _body = "File not found\n";
        return;
    }

    if (access(filepath.c_str(), W_OK) != 0)
    {
        _status = 403;
        _body = "Permission denied\n";
        return;
    }

    int fd = open(filepath.c_str(), O_WRONLY | O_TRUNC);
    if (fd == -1)
    {
        _status = 500;
        _body = "Failed to open file\n";
        return;
    }
    close(fd);

    _status = 200;
    _body = "File deleted successfully\n";
}

std::string Response::build()
{
  std::string response;

  initStatusCodes(_mapStatusCodes);
  initMediaTypes(_mapMediaTypes);

  if (_req.getMethod() == "GET")
      Get();
  else if (_req.getMethod() == "POST")
      Post();
  else if (_req.getMethod() == "DELETE")
      Delete();
  else
  {
    _status = 405;
    throw ResponseException("405 Method Not Allowed");
  }

  initHeaders(_headers);
  response = mergeResponseToString();
  return response;
}

std::string Response::build(int status)
{
    std::string response; 
    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);
    serveErrorPage(status);
    initHeaders(_headers);
    response = mergeResponseToString();
    return response;
}

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
        std::make_pair(".htm",  "text/html"),
        std::make_pair(".css",  "text/css"),
        std::make_pair(".csv",  "text/csv"),
        std::make_pair(".js",   "application/javascript"),

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
