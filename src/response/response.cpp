#include "../../includes/container.hpp"

std::string Response::cookieHeaderBuilder()
{
    if (_req.getCookies() == NULL)
        return "";

    Session* session  = _req.getCookies();
    time_t   now      = time(NULL);

    if (session->getExpiresAt() <= now)
        return "";

    std::string cookieValue = "session_id=" + session->getId();

    const std::map<std::string, std::string>& data = session->getData();
    for (std::map<std::string, std::string>::const_iterator it = data.begin();
         it != data.end(); ++it)
    {
        cookieValue += "; " + it->first + "=" + it->second;
    }

    cookieValue += "; Expires=" + getHttpDate(session->getExpiresAt());
    cookieValue += "; Path=/";
    cookieValue += "; HttpOnly";

    return cookieValue;
}

void Response::initHeaders(std::map<std::string, std::string>& headers)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    headers.insert(std::make_pair("Date",       getHttpDate(ts.tv_sec)));
    headers.insert(std::make_pair("Server",     RespDefaults::SERVER_NAME));
    headers.insert(std::make_pair("Connection", "close"));

    // Content-Type
    std::string contentType = "text/plain";
    if (_req.getMethod() == "GET" && _status < 400)
        contentType = returnMediaType(_req.getPath());
    else if (_status >= 400 && !_body.empty())
        contentType = "text/html";
    headers.insert(std::make_pair("Content-Type", contentType));

    // Redirect Location
    if (_status == 301 || _status == 302)
        headers.insert(std::make_pair("Location", _loc->getReturnUrl()));

    // Cookie
    headers.insert(std::make_pair("Set-Cookie", cookieHeaderBuilder()));

    // Body / File headers
    if (_req.getMethod() == "GET" && _status < 400 && _file_fd >= 0)
    {
        struct stat st;
        if (fstat(_file_fd, &st) == 0)
        {
            headers.insert(std::make_pair("Last-Modified",  getHttpDate(st.st_mtim.tv_sec)));
            headers.insert(std::make_pair("Content-Length", to_string98(st.st_size)));
        }
    }
    else
    {
        if (_req.getMethod() == "POST" && _status == 201)
            headers.insert(std::make_pair("Location", _req.getPath()));
        headers.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    }
}

void Response::serveErrorPage(int status)
{
    _status = status;

    const std::string codeStr = to_string98(status);

    const std::map<int, std::string>& conf = _req.getServerConf().getErrorPages();
    std::map<int, std::string>::const_iterator confIt = conf.find(status);

    if (confIt != conf.end())
    {
        std::string customPath = RespDefaults::ROOT + confIt->second;
        _body = ft_readFile(customPath);
    }

    bool isCustom = !_body.empty();
    if (!isCustom)
    {
        _body = ft_readFile(RespDefaults::ERROR_PAGE);
        if (_body.empty())
        {
            _body = codeStr + " Error";
            return;
        }
    }

    if (!isCustom)
    {
        std::string msgStr = "Unknown Error";
        std::map<int, std::string>::iterator statusIt = _mapStatusCodes.find(status);
        if (statusIt != _mapStatusCodes.end())
            msgStr = statusIt->second;

        for (size_t pos = 0;
             (pos = _body.find(RespDefaults::CODE_TAG, pos)) != std::string::npos; )
        {
            _body.replace(pos, RespDefaults::CODE_TAG.size(), codeStr);
            pos += codeStr.length();
        }
        for (size_t pos = 0;
             (pos = _body.find(RespDefaults::MESSAGE_TAG, pos)) != std::string::npos; )
        {
            _body.replace(pos, RespDefaults::MESSAGE_TAG.size(), msgStr);
            pos += msgStr.length();
        }
    }
}

bool isDirectory(const std::string& path) {
    // Explicit trailing slash → directory
    if (!path.empty() && path[path.size() - 1] == '/')
        return true;

    // No dot after last slash → treat as directory
    size_t lastSlash = path.rfind('/');
    size_t lastDot   = path.rfind('.');
    
    if (lastDot == std::string::npos)
        return true;                    // no extension → directory
    if (lastSlash != std::string::npos && lastDot < lastSlash)
        return true;                    // dot is in a dir component, not filename

    return false;
}

void Response::Get()
{
    if (!isMethodAllowed("GET"))
        return;

    const std::string root = (_loc && !_loc->getRoot().empty())
                           ? _loc->getRoot()
                           : _req.getServerConf().getRoot();

    // TODO:
    // should update this to work for all /cgi and /cgi/ and /cgi/index.html for example
    std::string filepath = root;
    //if (!isDirectory(_req.getPath()))
    filepath += getFileName(_req.getPath());
    /*
    if (access(filepath.c_str(), F_OK) != 0)
    {
        serveErrorPage(404);
        return;
    }*/

    struct stat info;
    if (stat(filepath.c_str(), &info) != 0)
    {
        serveErrorPage(500);
        return;
    }

    if (S_ISDIR(info.st_mode))
    {
        const std::vector<std::string>& indexList =
            (_loc && !_loc->getIndex().empty())
                ? _loc->getIndex()
                : _req.getServerConf().getIndex();

        bool indexFound = false;
        for (size_t i = 0; i < indexList.size(); ++i)
        {
            std::string indexPath = filepath + "/" + indexList[i];
            if (access(indexPath.c_str(), F_OK) == 0)
            {
                filepath    = indexPath;
                indexFound  = true;
                break;
            }
        }

        if (!indexFound)
        {
            const bool autoindex = (_loc)
                                    ? _loc->getAutoindex()
                                    : _req.getServerConf().getAutoIndex();
            if (!autoindex)
            {
                serveErrorPage(403);
                return;
            }
            _status = 200;
            _body   = generateAutoIndex(filepath);
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

    _status = 200;
}

void Response::Post()
{
    if (!isMethodAllowed("POST"))
        return;

    const size_t maxSize = (_loc)
                            ? _loc->getClientMaxBodySize()
                            : _req.getServerConf().getClientMaxBodySize();

    const std::string contentType   = _req.getHeaderValue("content-type");

    if (!isSupportedContentType(contentType, _mapMediaTypes))
    {
        serveErrorPage(415);
        return;
    }
    if (_req.getContentLen() > maxSize)
    {
        serveErrorPage(413);
        return;
    }

    const std::string uploadStore = (_loc && !_loc->getUploadStore().empty())
                                     ? _loc->getUploadStore()
                                     : RespDefaults::UPLOAD_STORE;

    const std::string filepath = uploadStore + "/" + generateUploadFileName(contentType);

    if (_req.isRequsetLarge())
    {
        int rf = open(_req.getTmpFileName().c_str(), O_RDONLY);
        if (rf <  0)
        {
          DEBUG_ERROR("yeah its less then 0");
          serveErrorPage(500);
          return;
        }
        else if (!transferToNewFile(fd, rf))
          serveErrorPage(500);
        std::remove(_req.getTmpFileName().c_str());
        close(rf);
    }
    else
    {
        size_t bytes = write(fd, _req.getBody().c_str(), _req.getBody().size());
        if (bytes != _req.getBody().size())
            serveErrorPage(500);
        // you can't use ostream for make file executable thats why we should use open 0755
        // std::ofstream file(filepath.c_str());
        // file << _req.getBody();
        // file.close();
    }
    close(fd); 
    if (_status != 500)
    {
        _status = 201;
        _body = "Created";
    }
}

void Response::Delete()
{
    if (!isMethodAllowed("DELETE"))
        return;

    std::string uploadStore = (_loc && !_loc->getUploadStore().empty())
                                  ? _loc->getUploadStore()
                                  : RespDefaults::UPLOAD_STORE;

    std::string filepath = uploadStore + getFileName(_req.getPath());

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

    if (std::remove(filepath.c_str()) != 0)
    {
        serveErrorPage(500);
        return;
    }

    _status = 200;
    _body = "File deleted successfully\n";
}

std::string Response::build()
{
    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);
    _loc = _req.getMatchLoc();

    if (!tryApplyLocationReturn())
    {
        const std::string method = _req.getMethod();
        if      (method == "GET")    Get();
        else if (method == "POST")   Post();
        else if (method == "DELETE") Delete();
        else                         serveErrorPage(405);
    }

    initHeaders(_headers);
    return mergeResponseToString();
}

std::string Response::build(int status)
{
    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);

    serveErrorPage(status);

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    _headers.insert(std::make_pair("Date",           getHttpDate(ts.tv_sec)));
    _headers.insert(std::make_pair("Server",         RespDefaults::SERVER_NAME));
    _headers.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    _headers.insert(std::make_pair("Connection",     "close"));
    _headers.insert(std::make_pair("Content-Type",   "text/html"));

    return mergeResponseToString();
}
