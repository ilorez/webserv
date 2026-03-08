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

void Request::setPath(const std::string &p)
{
  if (p == "/" || p == "/index.html")
    _path = "./www/index.html";
  else
    _path = p;
  /* ALAOUI: If the path is invalid, keep the link unchanged */
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

  setPath(_path); // ? for testing
}

/*
 ?
  GET /index.html HTTP/1.1\r\n
  Host: example.com\r\n
  User-Agent: Mozilla/5.0\r\n
  Accept: /*/ /*\r\n
  Connection: close\r\n
  \r\n
*/