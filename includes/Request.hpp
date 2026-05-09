#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "serverConfig.hpp"
#include "locationConfig.hpp"


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
  int          _tmp_fd;
  std::string  _tmp_file_name;
  size_t       _bytes_counter;
  bool         _is_request_large;

  ServerConfig _servers;
public:
  Request();
  Request(const Request &other);
  Request &operator=(const Request &other);
  ~Request();

  // getters
  std::string getVersion() const;
  std::string getMethod() const;
  size_t getContentLen() const;
  std::string getPath() const;
  std::string getHeaderValue(std::string key);
  const std::string&  getTmpFileName()  const;
  int  getTmpFd() const;
  bool                isRequsetLarge() const;
  size_t getBytesCounter() const;

  // TODO
  std::string getBody() const;
  ServerConfig getServer() { return _servers; };

  // setters
  void  setTmpFileName(std::string name);
  void  setTmpFd(int fd);
  void  setIsRequestLarge(bool value);
  void  setBytesCounter(size_t bytes);
  void  incrementBytesCounter(size_t bytes);
  void setPath(const std::string &path);
  void requestParser(const std::string &request);
  //TODO
  void setBody(std::string &value);
  const LocationConfig* getMatchedLocation() const;
  // methods
  bool isCGI();

private:
  // parse request
  void _parseFirstLine(const std::vector<std::string> &lines);
  void _parseAllHeaders(const std::vector<std::string> &lines);
  void _parseHeader(const std::string &key, const std::string &value);
  void _insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders);
};

#endif
