#include "../../includes/container.hpp"
#include <cstddef>
#include <string>

Request::Request():_content_size(0), _tmp_fd(-1), _tmp_file_name(""), _bytes_counter(0),_is_request_large(false)
{}

Request::Request(const Request &other) : _path(other._path), _version(other._version), _method(other._method), _headers(other._headers) {};

Request &Request::operator=(const Request &other)
{
  if (this != &other)
  {
    this->_method = other._method;
    this->_path = other._path;
  }
  return *this;
}

Request::~Request() {
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

const std::string & Request::getTmpFileName() const
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

// ? member functions

static void initCommaHeaders(std::set<std::string> &commaHeaders)
{
  commaHeaders.insert("accept");
  commaHeaders.insert("accept-charset");
  commaHeaders.insert("accept-encoding");
  commaHeaders.insert("accept-language");
  commaHeaders.insert("accept-ranges");
  commaHeaders.insert("allow");
  commaHeaders.insert("cache-control");
  commaHeaders.insert("connection");
  commaHeaders.insert("content-encoding");
  commaHeaders.insert("content-language");
  commaHeaders.insert("if-match");
  commaHeaders.insert("if-none-match");
  commaHeaders.insert("pragma");
  commaHeaders.insert("te");
  commaHeaders.insert("trailer");
  commaHeaders.insert("transfer-encoding");
  commaHeaders.insert("upgrade");
  commaHeaders.insert("vary");
  commaHeaders.insert("via");
  commaHeaders.insert("warning");
}

void Request::_parseHeader(const std::string &key, const std::string &value)
{
  std::string forbiddenChars = " \"(),/:;<=>?@[]{}\\";

  if (key.empty() || value.empty())
    throw RequestException("400 Bad Request");

  // ? key grammar check
  if (key.find_first_of(forbiddenChars) != std::string::npos)
    throw RequestException("400 Bad Request");
}

void Request::_insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders)
{
  std::transform(key.begin(), key.end(), key.begin(),
                 ::toLowerCase); // convert the entire string to lowercase. as the key is case-insensitive

  if (key == "set-cookie") // store it in a seperated vector, as its an exception
    _setCookieHeaders.push_back(std::pair<std::string, std::string>(key, value));
  else if (_headers.find(key) != _headers.end()) // concat with comma, otherwise ignore new ones
  {
    if (commaHeaders.find(key) != commaHeaders.end())
      _headers.find(key)->second += ", " + value;
    else
      return;
  }
  else // store normal headers
    _headers.insert(std::pair<std::string, std::string>(key, value));
}

void Request::_parseAllHeaders(const std::vector<std::string> &lines)
{
  std::set<std::string> commaHeaders;
  std::string key, value;

  initCommaHeaders(commaHeaders);
  for (size_t i = 1; i < lines.size(); i++)
  {
    if (lines[i].empty())
      throw RequestException("400 Bad Request");

    key = lines[i].substr(0, lines[i].find_first_of(":"));
    value = trim(lines[i].substr(lines[i].find_first_of(":") + 1));

    _parseHeader(key, value);
    _insertHeader(key, value, commaHeaders);
  }
}

void Request::_parseFirstLine(const std::vector<std::string> &lines)
{
  std::vector<std::string> fields;
  std::string method, path, version, first_line;

  first_line = lines.front();
  split(first_line, fields, " ");
  if (fields.size() != 3)
    throw RequestException("400 Bad Request");

  method = fields[0];
  path = fields[1]; // ? i could check for the length of the uri, if its too long, throw 414 URI Too Long
  version = fields[2];
  if (!path.empty() && (method == "GET" || method == "POST" || method == "DELETE") && version == "HTTP/1.1") // todo : i will add the rest of the methods later
  {
    _path = path;
    _method = method;
    _version = version;
  }
  else
    throw RequestException("400 Bad Request");
}

void Request::requestParser(const std::string &raw)
{
  std::vector<std::string> lines;
  std::string del = "\r\n";

  split(raw, lines, del);

  _parseFirstLine(lines);
  _parseAllHeaders(lines);
  // NOTE: importent to add request methods that have body here like "put" if you use it
  if (_method == "POST" && !to_integer<std::string, size_t>(getHeaderValue("Content-Length"), _content_size))
  {
    DEBUG_ERROR("request parser: invalid Content-Length");
    throw RequestException("400 Bad Request");
  }
}

bool Request::isCGI()
{
  return (_path.find("cgi") != std::string::npos);
}
