#include "../../includes/container.hpp"

Response::Response()
    : _file_fd(-1)
{}

Response::Response(const Response &other) : _req(other._req), _file_fd(other._file_fd)
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

Response::~Response() {};


void Response::setReq(Request &req)
{
  this->_req = req;
}

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
    const LocationConfig* loc = _req.getMatchedLocation();


    h.insert(std::make_pair("Date", getHttpDate(ts.tv_sec)));
    h.insert(std::make_pair("Server", "MyServer"));
    h.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    h.insert(std::make_pair("Connection", "close"));

    std::string contentType = "text/plain";

    if (_req.getMethod() == "GET")
        contentType = returnMediaType(_req.getPath());

    h.insert(std::make_pair("Content-Type", contentType));

    if (_req.getMethod() == "GET")
    {
        std::string filepath;

        if (_status >= 400 && _status < 600)
        {
            filepath = "./www/errorPage.html";
        }
        else
        {
            std::string root = (loc && !loc->getRoot().empty()) ? loc->getRoot() : _req.getServerConf().getRoot();
            filepath = root + _req.getPath();
        }

        struct stat st;
        if (stat(filepath.c_str(), &st) != -1)
            h.insert(std::make_pair("Last-Modified", getHttpDate(st.st_mtim.tv_sec)));
    }
    else if (_req.getMethod() == "POST")
    {
        if (_status == 201)
            h.insert(std::make_pair("Location", _req.getPath()));
    }
}

std::string  Response::mergeResponseToString()
{
    std::string ret;

    ret += getHeaders();
    ret += "\r\n";
    ret += getBody();

    return (ret);
}

void Response::serveErrorPage(int status)
{
    _status = status;

    const std::map<int, std::string>& conf = _req.getServerConf().getErrorPages();
    std::string errorPage;
    bool isCustom = false;

    std::map<int, std::string>::const_iterator it = conf.find(status);
    if (it != conf.end())
    {
        errorPage = "./www" + it->second;
        isCustom = true;
    }
    else
        errorPage = "./www/errorPage.html";

    _body = ft_readFile(errorPage);
    if (_body.empty())
    {
        std::string codeStr = to_string98(status);
        _body = codeStr + " Error";
        return;
    }

    // custom page
    if (isCustom)
        return;

    // default page: replace {{CODE}} and {{MESSAGE}}
    std::string codeStr = to_string98(status);
    std::string msgStr = "Unknown Error";

    std::map<int, std::string>::iterator it2 = _mapStatusCodes.find(status);
    if (it2 != _mapStatusCodes.end())
        msgStr = it2->second;

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
    const LocationConfig* loc = _req.getMatchedLocation();

    if (loc && loc->hasReturn())
    {
        _status = loc->getReturnCode();
        _body = loc->getReturnUrl();
        return;
    }

    if (loc && !loc->getMethods().empty() && std::find(loc->getMethods().begin(), loc->getMethods().end(), "GET") == loc->getMethods().end())
    {
        serveErrorPage(405);
        return;
    }

    std::string path = _req.getPath();
    std::string root = (loc && !loc->getRoot().empty()) ? loc->getRoot() : _req.getServerConf().getRoot();
    std::string filepath = root + path;

    // existence
    std::cout << "!! --- !! check filepath : " << filepath << std::endl;
    if (access(filepath.c_str(), F_OK) != 0)
    {
        serveErrorPage(404);
        return;
    }

    struct stat info;
    stat(filepath.c_str(), &info);
    if (S_ISDIR(info.st_mode))
    {
        std::vector<std::string> indexList =
            (loc && !loc->getIndex().empty()) ? loc->getIndex() : _req.getServerConf().getIndex();

        bool found = false;
        for (size_t i = 0; i < indexList.size(); ++i)
        {
            std::string indexPath = filepath + "/" + indexList[i];
            if (access(indexPath.c_str(), F_OK) == 0)
            {
                filepath = indexPath;
                found = true;
                break;
            }
        }

        if (!found)
        {
            bool autoindex = (loc) ? loc->getAutoindex() : _req.getServerConf().getAutoIndex();
            if (!autoindex)
            {
                serveErrorPage(403);
                return;
            }
            _status = 200;
            _body = generateAutoIndex(filepath, path);
            return;
        }
    }

    if (access(filepath.c_str(), R_OK) != 0)
    {
        serveErrorPage(403);
        return;
    }

    _file_fd = open(filepath.c_str(), O_RDONLY);
    if (_file_fd < 0)
    {
        serveErrorPage(404);
        return;
    }
    
    _body = ft_readFile(filepath); // alaoui::todo, i remove that line   
    _status = 200;
}

void Response::Post()
{
    const LocationConfig* loc = _req.getMatchedLocation();

    if (loc && loc->hasReturn())
    {
        _status = loc->getReturnCode();
        _body = loc->getReturnUrl();
        return;
    }

    if (loc && !loc->getMethods().empty() && std::find(loc->getMethods().begin(), loc->getMethods().end(), "POST") == loc->getMethods().end())
    {
        serveErrorPage(405);
        return;
    }

    size_t max_size = (loc) ? loc->getClientMaxBodySize() : _req.getServerConf().getClientMaxBodySize();
    std::string contentLengthStr = _req.getHeaderValue("content-length");

    if (!contentLengthStr.empty() && std::atoi(contentLengthStr.c_str()) > (int)max_size)
    {
        serveErrorPage(413);
        return;
    }

    std::string filepath = _req.getServerConf().getRoot() + _req.getPath();
    if (_req.isRequsetLarge())
    {
        // for large files
        int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            serveErrorPage(500);
            return;
        }
        _req.setTmpFileName(filepath);
        _req.setTmpFd(fd);
    }
    else
    {
        std::ofstream file(filepath.c_str());
        if (!file)
        {
            serveErrorPage(500);
            return;
        }
        file << _req.getBody();
        file.close();
        _status = 201;
        _body = "Created";
    }
}

void Response::Delete()
{
    const LocationConfig* loc = _req.getMatchedLocation();

    if (loc && loc->hasReturn())
    {
        _status = loc->getReturnCode();
        _body = loc->getReturnUrl();
        return;
    }

    if (loc && !loc->getMethods().empty() && std::find(loc->getMethods().begin(), loc->getMethods().end(), "DELETE") == loc->getMethods().end())
    {
        serveErrorPage(405);
        return;
    }

    std::string filepath = _req.getServerConf().getRoot() + _req.getPath();

    if (access(filepath.c_str(), F_OK) != 0)
    {
        serveErrorPage(404);
        return;
    }

    if (access(filepath.c_str(), W_OK) != 0)
    {
        serveErrorPage(403);
        return;
    }

    int fd = open(filepath.c_str(), O_WRONLY | O_TRUNC);
    if (fd == -1)
    {
        serveErrorPage(500);
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
    serveErrorPage(405);

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

std::string Response::generateAutoIndex(const std::string& fullPath, const std::string& uriPath)
{
    DIR* dir = opendir(fullPath.c_str());
    if (!dir)
        return "";

    std::string body;
    body += "<html><body><h1>Index of " + uriPath + "</h1><br>";

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // skip . and ..
        if (name == "." || name == "..")
            continue;

        std::string displayName = name;
        std::string href = uriPath;
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

std::string Response::getBody()
{
    return (_body);
}

int         Response::getBodyFd()
{
    return (_file_fd);
}
