#ifndef REQUEST_HPP
#define REQUEST_HPP

class Request
{
private:
  std::string _path;                           // /, /index.html
  std::string _version;                        // HTTP/1.1
  std::string _method;                         // get, post, delete
  std::map<std::string, std::string> _headers; // http headers
  std::vector<std::pair<std::string, std::string>> _setCookieHeaders;

public:
  Request(const std::string &raw);
  Request(const Request &other);
  Request &operator=(const Request &other);
  ~Request();

  // getters and setters
  std::string getVersion() const;
  std::string getMethod() const;
  std::string getPath() const;
  std::string getHeaderValue(const std::string &key) const;

  void setPath(const std::string &path);

private:
  // parse request
  void _requestParser(const std::string &request);
  void _parseFirstLine(const std::vector<std::string> &lines);
  void _parseAllHeaders(const std::vector<std::string> &lines);
  void _parseHeader(const std::string &key, const std::string &value);
  void _insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders);
};

#endif
