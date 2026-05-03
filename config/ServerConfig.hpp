#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

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
#include <cstdint>

class LocationConfig;

using namespace std;

class ServerConfig
{
private:
	std::string _host;
	uint16_t _port;
	std::string _serverName;
	std::string _root;
	unsigned long _clientMaxBodySize;
	std::vector<std::string> _index;
	bool _autoindex;

	std::map<int, string> _errorPage;

	vector<LocationConfig> _locations;

public:
	// constructor & destructor
	ServerConfig();
	~ServerConfig();

	// getters and setters
	void setHost(const string &host);
	void setPort(const std::string &port);
	void setServerName(const string &serverName);
	void setRoot(const string &root);
	void setClientMaxBodySize(const std::string &clientMaxBodySize);
	void setErrorPages(const std::vector<std::string> &errorPage);
	void setAutoIndex(const std::string &index);
	void setIndex(const std::vector<std::string> &index);
	void setLocation(const LocationConfig &locationBlock);

	// getters
	const std::string &getHost() const;
	uint16_t getPort() const;
	const std::string &getServerName() const;
	const std::string &getRoot() const;
	unsigned long getClientMaxBodySize() const;
	const std::vector<std::string> &getIndex() const;
	bool getAutoIndex() const;
	const std::map<int, std::string> &getErrorPages() const;
	const std::vector<LocationConfig> &getLocations() const;

	// helpers
	bool isValidStatusCode(const std::string &str);
	bool isValidPath(const std::string &path);
};

#endif