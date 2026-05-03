
#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

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


using namespace std;

class ServerConfig;

class LocationConfig
{
private:
	std::string _path;
	std::string _root;
	std::vector<std::string> _methods;
	std::vector<std::string> _index;
	bool _autoindex;
	// std::string _returnUrl;
	// int _returnCode;
	unsigned long _clientMaxBodySize;

public:
	LocationConfig(const ServerConfig &serverConfig);

	~LocationConfig();

	// getters
	const std::string &getPath() const;
	const std::string &getRoot() const;
	const std::vector<std::string> &getMethods() const;
	const std::vector<std::string> &getIndex() const;
	bool getAutoindex() const;
	// const std::string &getReturnUrl() const;
	// int getReturnCode() const;
	unsigned long getClientMaxBodySize() const;

	// setter
	void setPath(const std::string &path);
	void setRoot(const std::string &root);
	void setMethods(const std::vector<std::string> &methods);
	void setIndex(const std::vector<std::string> &index);
	void setAutoIndex(const std::string autoindex);
	// void setReturnUrl(const std::string &url);
	// void setReturnCode(int code);
	void setClientMaxBodySize(const string &clientMaxBodySize);

	// helpers
	// void addMethod(const std::string &method) { _methods.push_back(method); }
	// void addIndex(const std::string &index) { _index.push_back(index); }

	bool isMethodAllowed(const std::string &method) const;
	bool isValidPath(const std::string &path);
	bool IsvalidStatusCode(const std::string &str);
};

#endif