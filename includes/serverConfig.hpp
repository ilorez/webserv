#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "EpollHold.hpp"
#include <iomanip>
#include <time.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <cctype>
#include <sys/stat.h>

class LocationConfig;

class ServerConfig
{
private:
  int _socket_fd;
	std::string _host; // ip
	unsigned int _port;
	std::string _serverName;
	std::string _root;
	unsigned long _clientMaxBodySize;
	std::vector<std::string> _index;
	bool _autoindex;
	std::map<int, std::string> _errorPage;
	std::vector<LocationConfig> _locations;
  t_epollhold _srv_hold;

public:
	// constructor & destructor
	ServerConfig();
	~ServerConfig();
  ServerConfig(const ServerConfig& other);
  ServerConfig& operator=(const ServerConfig& other);

	// getters and setters
	void setHost(const std::string &host, size_t line);
	void setPort(const std::string &port, size_t line);
	void setServerName(const std::string &serverName);
	void setRoot(const std::string &root, size_t line);
	void setClientMaxBodySize(const std::string &clientMaxBodySize, size_t line);
	void setErrorPages(const std::vector<std::string> &errorPage, size_t line);
	void setAutoIndex(const std::string &index, size_t line);
	void setIndex(const std::vector<std::string> &index, size_t line);
	void setLocation(const LocationConfig &locationBlock);
  void setSocketFd(const int _socket_fd);

	// getters
  int getFd() const; // get socket fd
	const std::string &getHost() const;
	unsigned int getPort() const;
	const std::string &getServerName() const;
	const std::string &getRoot() const;
	unsigned long getClientMaxBodySize() const;
	const std::vector<std::string> &getIndex() const;
	bool getAutoIndex() const;
	const std::map<int, std::string> &getErrorPages() const;
	const std::vector<LocationConfig> &getLocations() const;
  t_epollhold& getServHold();

	// helpers
};

#endif
