#include "../../includes/container.hpp"

Request::Request() : _content_size(0), _tmp_fd(-1), _tmp_file_name(""), _bytes_counter(0), _is_request_large(false), _is_cgi(false)
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
    this->_setCookieHeaders = other._setCookieHeaders;
    this->_tmp_fd = other._tmp_fd;
    this->_tmp_file_name = other._tmp_file_name;
    this->_bytes_counter = other._bytes_counter;
    this->_is_request_large = other._is_request_large;
    this->_serverConf = other._serverConf;
    this->_is_cgi = other._is_cgi;
  }
  return *this;
}

Request::~Request()
{
  if (_tmp_fd >= 0)
    close(_tmp_fd);
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
  return (_tmp_file_name);
}
int Request::getTmpFd() const
{
  return (_tmp_fd);
}

bool Request::isRequsetLarge() const
{
  return (_is_request_large);
}

size_t Request::getBytesCounter() const
{
  return (_bytes_counter);
}

std::string Request::getBody() const
{
  return _body;
}

const ServerConfig &Request::getServerConf() const
{
  return _serverConf;
};

// ? setters

void Request::setPath(const std::string &path)
{
  this->_path = path;
}

void Request::setTmpFd(int fd)
{
  _tmp_fd = fd;
}
void Request::setTmpFileName(std::string name)
{
  _tmp_file_name = name;
}

void Request::setIsRequestLarge(bool value)
{
  _is_request_large = value;
}

void Request::setBytesCounter(size_t bytes)
{
  _bytes_counter = bytes;
}

void Request::incrementBytesCounter(size_t bytes)
{
  _bytes_counter += bytes;
}

void Request::setBody(const std::string &value)
{
  _body = value;
}

void Request::setServerConfig(ServerConfig &sc)
{
  _serverConf = sc;
}
