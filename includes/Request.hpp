#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#include <vector>

class Request
{
private:
  size_t _content_size;
  std::string _path;                           // /, /index.html
  std::string _version;                        // HTTP/1.1
  std::string _method;                         // get, post, delete
  std::string _body;
  std::map<std::string, std::string> _headers; // http headers
  std::vector<std::pair<std::string, std::string> > _setCookieHeaders;

public:
  Request();
  Request(const std::string &raw);
  Request(const Request &other);
  Request &operator=(const Request &other);
  ~Request();

  // getters and setters
  std::string getVersion() const;
  std::string getMethod() const;
  size_t getContentLen() const;
  std::string getPath() const;
  std::string getHeaderValue(std::string key);

  void setPath(const std::string &path);
  void requestParser(const std::string &request);
private:
  // parse request
  void _parseFirstLine(const std::vector<std::string> &lines);
  void _parseAllHeaders(const std::vector<std::string> &lines);
  void _parseHeader(const std::string &key, const std::string &value);
  void _insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders);
};

#endif
