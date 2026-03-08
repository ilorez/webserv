#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "iostream"
#include "map"
#include "vector"

class Request
{
private:
  std::string _path;                           // /, /index.html
  std::string _version;                        // HTTP/1.1
  std::string _method;                         // get, post, delete
  std::map<std::string, std::string> _headers; // http headers
  std::string _body;                           // for post
public:
  Request(const std::string &raw);
  Request(const Request &other);
  Request &operator=(const Request &other);
  ~Request();

  // getters and setters
  std::string getMethod() const;
  std::string getPath() const;
  std::string getBody() const;

  // set path
  void setPath(const std::string &);

private:
  // parse request
  void _request_pars(const std::string &request);
  void _parse_first_line(const std::vector<std::string> &lines);
  void _parse_headers(const std::vector<std::string> &lines);
};

#endif
