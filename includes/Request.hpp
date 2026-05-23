#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"

class Request
{
private:
  size_t _content_size;
  std::string _path;    // /, /index.html
  std::string _version; // HTTP/1.1
  std::string _method;  // get, post, delete
  std::string _body;
  std::map<std::string, std::string> _headers; // http headers
  int _tmpFd;
  std::string _tmpFileName;
  size_t _bytesCounter;
  bool _isRequestLarge;
  ServerConfig _serverConf;
  bool _isCgi;
  const LocationConfig *_matchLoc; // request match location, TODO: Alaoui should use this
  std::string _filePath;           // this is the file path of cgi, you can use it for request too, i use it only in case of CGI run request
  std::string _cgiPath;

  Session *_session; // cookies
  bool _isNewSession;

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
  const std::string &getTmpFileName() const;
  int getTmpFd() const;
  bool isRequsetLarge() const;
  size_t getBytesCounter() const;
  std::string getFilePath() const;
  const LocationConfig *getMatchLoc() const;
  std::map<std::string, std::string> &getHeaders();
  Session *getSession();
  bool isNewSession();
  std::string getCgiPath() const;

  // TODO
  std::string getBody() const;
  const ServerConfig &getServerConf() const;

  // setters
  void setTmpFileName(std::string name);
  void setTmpFd(int fd);
  void setIsRequestLarge(bool value);
  void setBytesCounter(size_t bytes);
  void incrementBytesCounter(size_t bytes);
  void setPath(const std::string &path);
  void requestParser(const std::string &headers);
  void set_cgiPath(const std::string &path);
  // TODO
  void setBody(const std::string &value);
  const LocationConfig *getMatchedLocation() const;
  bool isMethodAllowed(const std::string &method);
  bool getIsNewSession();

  // methods
  bool isCGI();
  void setServerConfig(ServerConfig &sc);

private:
  // parse request
  void _parseFirstLine(const std::vector<std::string> &lines);
  void _parseAllHeaders(const std::vector<std::string> &lines);
  void _parseHeader(const std::string &key, const std::string &value);
  void _insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders);

  // parse cookies
  void parseCookies();
};

#endif
