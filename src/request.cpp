#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include "../includes/debug.hpp"
#include "../includes/container.hpp"

using namespace std;

// ? Canonical Form
Request::Request(const std::string &raw)
{
  this->_request_pars(raw);
}

Request::Request(const Request &other) : _path(other._path), _version(other._version), _method(other._method), _headers(other._headers), _body(other._body) {}

Request &Request::operator=(const Request &other)
{
  if (this != &other)
  {
    this->_method = other._method;
    this->_path = other._path;
    this->_body = other._body;
  }
  return *this;
}

Request::~Request() {};

// ?getters
std::string Request::getMethod() const
{
  return (this->_method);
}

std::string Request::getPath() const
{
  return (this->_path);
}

std::string Request::getBody() const
{
  return (this->_body);
}

std::string Request::getVersion() const
{
  return (this->_version);
}

void Request::_parse_headers(const std::vector<std::string> &lines)
{
  for (size_t i = 1; i < lines.size() - 2; i++) //! - 2 : not including the last 2 empty lines
  {
    if (lines[i].empty())
      throw RequestException("400 Bad Request");

    string key = trim(lines[i].substr(0, lines[i].find_first_of(":")));
    string value = trim(lines[i].substr(lines[i].find_first_of(":") + 1));
    if (key.empty() || key.empty())
      throw RequestException("400 Bad Request");
    _headers.insert(pair<string, string>(key, value));
  }
}

void Request::_parse_first_line(const std::vector<std::string> &lines)
{
  vector<string> fields;
  std::string method, path, version, first_line;

  first_line = lines.front();
  split(first_line, fields, " ");

  method = fields[0];
  path = fields[1];
  version = fields[2];
  if (!path.empty() && (method == "GET") && (version == "HTTP/1.0" || version == "HTTP/1.1")) // todo : i will add the rest of the methods later
  {
    _path = path;
    _method = method;
    _version = version;
  }
  else
    throw RequestException("400 Bad Request");
}

// ? member functions
void Request::_request_pars(const std::string &raw)
{
  std::vector<string> lines;
  std::string del = "\r\n";

  split(raw, lines, del);

  _parse_first_line(lines);
  _parse_headers(lines);
}