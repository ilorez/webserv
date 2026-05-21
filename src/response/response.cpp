#include "../../includes/container.hpp"

int gn = 0;

std::string Response::cookieHeaderBuilder()
{
    std::string cookieValue;
    Session *session = _req.get_session();
    time_t now = time(NULL);

    if (session->getExpiresAt() > now)
    {
        cookieValue = "session_id=" + session->getId();

        time_t exp = session->getExpiresAt();
        std::string time = getHttpDate(exp);
        std::map<std::string, std::string> mapp = _req.get_session()->getData();
        for (std::map<std::string, std::string>::iterator it = mapp.begin(); it != mapp.end(); it++)
        {
            cookieValue += "; " + it->first + "=" + it->second;
        }

        cookieValue += "; Expires=" + time;
        cookieValue += "; Path=/";
        cookieValue += "; HttpOnly";
    }
    return cookieValue;
}

void Response::initHeaders(std::map<std::string, std::string> &h)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    h.insert(std::make_pair("Date", getHttpDate(ts.tv_sec)));
    h.insert(std::make_pair("Server", RespDefaults::SERVER_NAME));
    h.insert(std::make_pair("Connection", "close"));

    std::string contentType = "text/plain";
    if (_req.getMethod() == "GET" && _status < 400)
        contentType = returnMediaType(_req.getPath());
    else if (_status >= 400 && !_body.empty())
        contentType = "text/html";
    h.insert(std::make_pair("Content-Type", contentType));

    if (_status == 301 || _status == 302)
        h.insert(std::make_pair("Location", _loc->getReturnUrl()));

    if (_req.is_new_session())
        h.insert(std::make_pair("Set-Cookie", cookieHeaderBuilder()));
    if (_req.getMethod() == "GET" && _status < 400 && _file_fd >= 0)
    {
        struct stat st;
        if (fstat(_file_fd, &st) == 0)
        {
            h.insert(std::make_pair("Last-Modified", getHttpDate(st.st_mtim.tv_sec)));
            h.insert(std::make_pair("Content-Length", to_string98(st.st_size)));
        }
    }
    else
    {
        if (_req.getMethod() == "POST" && _status == 201)
            h.insert(std::make_pair("Location", _req.getPath()));
        h.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    }
}

void Response::serveErrorPage(int status)
{
    _status = status;

    const std::map<int, std::string> &conf = _req.getServerConf().getErrorPages();
    std::string errorPage;
    bool isCustom = false;

    std::map<int, std::string>::const_iterator it = conf.find(status);
    if (it != conf.end())
    {
        errorPage = RespDefaults::ROOT + it->second;
        isCustom = true;
        _body = ft_readFile(errorPage);
    }

    if (_body.empty())
    {
        errorPage = RespDefaults::ERROR_PAGE;
        _body = ft_readFile(errorPage);
        isCustom = false;
        if (_body.empty())
        {
            std::string codeStr = to_string98(status);
            _body = codeStr + " Error";
            return;
        }
    }

    if (!isCustom)
    {
        std::string codeStr = to_string98(status);
        std::string msgStr = "Unknown Error";

        std::map<int, std::string>::iterator it2 = _mapStatusCodes.find(status);
        if (it2 != _mapStatusCodes.end())
            msgStr = it2->second;

        size_t pos = 0;
        while ((pos = _body.find(RespDefaults::CODE_TAG, pos)) != std::string::npos)
        {
            _body.replace(pos, 8, codeStr);
            pos += codeStr.length();
        }
        pos = 0;
        while ((pos = _body.find(RespDefaults::MESSAGE_TAG, pos)) != std::string::npos)
        {
            _body.replace(pos, 11, msgStr);
            pos += msgStr.length();
        }
    }
    return;
}

void Response::Get()
{
    if (!isMethodAllowed("GET"))
        return;

    std::string root = (_loc && !_loc->getRoot().empty())
                           ? _loc->getRoot()
                           : _req.getServerConf().getRoot();

    std::string filepath = root + getFileName(_req.getPath());
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
            (_loc && !_loc->getIndex().empty())
                ? _loc->getIndex()
                : _req.getServerConf().getIndex();

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
            bool autoindex = (_loc)
                                 ? _loc->getAutoindex()
                                 : _req.getServerConf().getAutoIndex();
            if (!autoindex)
            {
                serveErrorPage(403);
                return;
            }
            _status = 200;
            _body = generateAutoIndex(filepath);
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

bool Response::isSupportedContentType(
    const std::string &contentType,
    const std::map<std::string, std::string> &mediaTypes)
{
    for (std::map<std::string, std::string>::const_iterator it = mediaTypes.begin();
         it != mediaTypes.end(); ++it)
    {
        if (it->second == contentType)
            return true;
    }
    return false;
}

void Response::Post()
{
    if (!isMethodAllowed("POST"))
        return;

    size_t max_size = (_loc)
                          ? _loc->getClientMaxBodySize()
                          : _req.getServerConf().getClientMaxBodySize();

    std::string contentLengthStr = _req.getHeaderValue("content-length");
    std::string contentTypeStr = _req.getHeaderValue("content-type");

    if (!isSupportedContentType(contentTypeStr, _mapMediaTypes))
    {
        serveErrorPage(415);
        return;
    }
    if (_req.getContentLen() > max_size)
    {
        serveErrorPage(413);
        return;
    }

    std::string uploadStore = (_loc && !_loc->getUploadStore().empty())
                                  ? _loc->getUploadStore()
                                  : RespDefaults::UPLOAD_STORE;

    std::string filepath = uploadStore + "/" + generateUploadFileName(contentTypeStr);
    int rf = open(_req.getTmpFileName().c_str(), O_RDONLY);
    if (rf < 0)
    {
        DEBUG_ERROR("yeah its less then 0");
        serveErrorPage(500);
        return;
    }
    int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd < 0)
    {
        serveErrorPage(500);
        close(rf);
        return;
    }
    if (_req.isRequsetLarge())
    {
        if (!transferToNewFile(fd, rf))
            serveErrorPage(500);
        std::remove(_req.getTmpFileName().c_str());
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
    close(rf);
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

    std::remove(filepath.c_str());

    _status = 200;
    _body = "File deleted successfully\n";
}

std::string Response::build()
{
    std::string response;

    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);
    _loc = _req.getMatchLoc();

    if (!tryApplyLocationReturn())
    {
        if (_req.getMethod() == "GET")
            Get();
        else if (_req.getMethod() == "POST")
            Post();
        else if (_req.getMethod() == "DELETE")
            Delete();
        else
            serveErrorPage(405);
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
    // NOTE: TODO: you should never use _req in this part of building page base on status
    // because the _req may not be builded itself
    //_loc = _req.getMatchedLocation();
    serveErrorPage(status);

    // --- Creat Headers
    // NOTE: server error page can't use _req
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    _headers.insert(std::make_pair("Date", getHttpDate(ts.tv_sec)));
    _headers.insert(std::make_pair("Server", RespDefaults::SERVER_NAME));
    _headers.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    _headers.insert(std::make_pair("Connection", "close"));
    _headers.insert(std::make_pair("Content-Type", "text/html"));
    // ------------
    response = mergeResponseToString();
    return response;
}
