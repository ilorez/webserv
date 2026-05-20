#include "../../includes/container.hpp"

void Response::initHeaders(std::map<std::string, std::string> &h)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    h.insert(std::make_pair("Date",           getHttpDate(ts.tv_sec)));
    h.insert(std::make_pair("Server",         Default::SERVER_NAME));
    h.insert(std::make_pair("Content-Length", to_string98(_body.size())));
    h.insert(std::make_pair("Connection",     "close"));

    std::string contentType = "text/plain";
    if (_req.getMethod() == "GET" && _status < 400)
        contentType = returnMediaType(_req.getPath());
    else if (_status >= 400 && !_body.empty())
        contentType = "text/html";
    h.insert(std::make_pair("Content-Type", contentType));

    if (_status == 301 || _status == 302)
        h.insert(std::make_pair("Location", _loc->getReturnUrl()));


    if (_req.getCookies() != NULL)
    {
        Session *session = _req.getCookies();
        time_t now = time(NULL);

        if (session->getExpiresAt() > now)
        {
            std::string cookieValue = "session_id=" + session->getId();

            // Append expiry time as HTTP date
            char timeBuf[128];
            time_t exp = session->getExpiresAt();
            struct tm *gmt = gmtime(&exp);
            gmt = gmtime(&exp);
            strftime(timeBuf, sizeof(timeBuf), "%a, %d %b %Y %H:%M:%S GMT", gmt);

            cookieValue += "; Expires=" + std::string(timeBuf);
            cookieValue += "; Path=/";
            cookieValue += "; HttpOnly";

            h.insert(std::make_pair("Set-Cookie", cookieValue));
        }
    }

    if (_req.getMethod() == "GET" && _status < 400 && _file_fd >= 0)
    {
        struct stat st;
        if (fstat(_file_fd, &st) == 0)
            h.insert(std::make_pair("Last-Modified", getHttpDate(st.st_mtim.tv_sec)));
    }

    if (_req.getMethod() == "POST" && _status == 201)
        h.insert(std::make_pair("Location", _req.getPath()));
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
        errorPage = "./www/" + it->second;
        isCustom = true;
        _body = ft_readFile(errorPage);
    }

    if (_body.empty())
    {
        errorPage = Default::ERROR_PAGE;
        _body = ft_readFile(errorPage);
        isCustom = false;
        if (_body.empty())
        {
            std::string codeStr = to_string98(status);
            _body = codeStr + " Error";
            return;
        }
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
    if (!isMethodAllowed("GET"))
        return;

    DEBUG_INFO("this request is GEEET");
    std::string root     = (_loc && !_loc->getRoot().empty()) 
                            ? _loc->getRoot() 
                            : _req.getServerConf().getRoot();
    
    std::string filepath = "./www" + _req.getPath();
    DEBUG_INFO("Full Path is");
    std::cout << filepath << std::endl;
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
    
    _body = ft_readFile(filepath); // alaoui::todo, i remove that line   
    _status = 200;
}

void Response::Post()
{
    if (!isMethodAllowed("POST"))
        return;

    size_t max_size = (_loc) 
                        ? _loc->getClientMaxBodySize() 
                        : _req.getServerConf().getClientMaxBodySize();

    std::string contentLengthStr = _req.getHeaderValue("content-length");
    if (!contentLengthStr.empty() && std::atoi(contentLengthStr.c_str()) > (int)max_size)
    {
        serveErrorPage(413);
        return;
    }

    std::string uploadStore = (_loc && !_loc->getUploadStore().empty()) 
                                ? _loc->getUploadStore() 
                                : Default::UPLOAD_STORE;

    std::string filepath = uploadStore + "/" +  generateUploadFileName();
    if (_req.isRequsetLarge())
    {
        int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0 || !transferToNewFile(fd, _req.getTmpFd()))
        {
            serveErrorPage(500);
            return ;
        }
        std::remove(_req.getTmpFileName().c_str());
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
    }
    
    _status = 201;
    _body = "Created";
}

void Response::Delete()
{
    if (!isMethodAllowed("DELETE"))
        return;

    std::string uploadStore = (_loc && !_loc->getUploadStore().empty()) 
                                ? _loc->getUploadStore() 
                                : Default::UPLOAD_STORE;

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

void printMap(const std::map<std::string, std::string>& h)
{
    std::map<std::string, std::string>::const_iterator it;

    for (it = h.begin(); it != h.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
}

std::string Response::build()
{
    std::string response;
    
    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);

    _loc = _req.getMatchedLocation();

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
    std::cout << "\n\n-----------------------!!-------------------------------------\n\n";
    printMap(_headers);
    std::cout << "\n\n-----------------------!!-------------------------------------\n\n";
    response = mergeResponseToString();
    return response;
}

std::string Response::build(int status)
{
    std::string response; 
    initStatusCodes(_mapStatusCodes);
    initMediaTypes(_mapMediaTypes);
    _loc = _req.getMatchedLocation();
    serveErrorPage(status);
    initHeaders(_headers);
    response = mergeResponseToString();
    return response;
}
