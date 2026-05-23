#include "../../includes/container.hpp"

Request::Request() : _content_size(0), _path(""), _tmpFd(-1), _tmpFileName(""), _bytesCounter(0), _isRequestLarge(false), _isCgi(false), _matchLoc(NULL), _filePath(""), _cgiPath(""), _session(NULL), _isNewSession(true)
{
}

Request::Request(const Request &other)
{
  *this = other;
}

Request &Request::operator=(const Request &other)
{
  if (this != &other)
  {
    this->_content_size = other._content_size;
    this->_path = other._path;
    this->_version = other._version;
    this->_method = other._method;
    this->_body = other._body;
    this->_headers = other._headers;
    this->_tmpFd = other._tmpFd;
    this->_tmpFileName = other._tmpFileName;
    this->_bytesCounter = other._bytesCounter;
    this->_isRequestLarge = other._isRequestLarge;
    this->_serverConf = other._serverConf;
    this->_isCgi = other._isCgi;
    this->_session = other._session;
    this->_matchLoc = other._matchLoc;
    this->_filePath = other._filePath;
    this->_cgiPath = other._cgiPath;
    this->_isNewSession = other._isNewSession;
  }
  return *this;
}

Request::~Request()
{
  if (_tmpFd >= 0)
    close(_tmpFd);
};

// ?getters
std::string Request::getMethod() const
{
  return (this->_method);
}

size_t Request::getContentLen() const
{
  return (this->_content_size);
}

std::string Request::getPath() const
{
  return (this->_path);
}

std::string Request::getVersion() const
{
  return (this->_version);
}

std::map<std::string, std::string> &Request::getHeaders()
{
  return (this->_headers);
}

std::string Request::getHeaderValue(std::string key)
{
  std::transform(key.begin(), key.end(), key.begin(),
                 ::toLowerCase);
  std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);

  if (it == this->_headers.end())
    return "";

  return (it->second);
}

const std::string &Request::getTmpFileName() const
{
  return (_tmpFileName);
}
int Request::getTmpFd() const
{
  return (_tmpFd);
}

bool Request::isRequsetLarge() const
{
  return (_isRequestLarge);
}

size_t Request::getBytesCounter() const
{
  return (_bytesCounter);
}

std::string Request::getBody() const
{
  return _body;
}

const ServerConfig &Request::getServerConf() const
{
  return _serverConf;
};

std::string Request::getFilePath() const
{
  return (_filePath);
}

const LocationConfig *Request::getMatchLoc() const
{
  return (_matchLoc);
}

// ? setters
void Request::setPath(const std::string &path)
{
  this->_path = path;
}

void Request::setTmpFd(int fd)
{
  _tmpFd = fd;
}
void Request::setTmpFileName(std::string name)
{
  _tmpFileName = name;
}

void Request::setIsRequestLarge(bool value)
{
  _isRequestLarge = value;
}

void Request::setBytesCounter(size_t bytes)
{
  _bytesCounter = bytes;
}

void Request::incrementBytesCounter(size_t bytes)
{
  _bytesCounter += bytes;
}

void Request::setBody(const std::string &value)
{
  _body = value;
}

void Request::setServerConfig(ServerConfig &sc)
{
  _serverConf = sc;
}

Session *Request::getSession()
{
  return _session;
};

bool Request::getIsNewSession()
{
  return _isNewSession;
}

std::string Request::getCgiPath() const
{
  return _cgiPath;
};

bool Request::isNewSession()
{
  return _isNewSession;
}

void Request::set_cgiPath(const std::string &path)
{
  this->_cgiPath = path;
};
