
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

class ServerConfig;

class LocationConfig
{
private:
	std::string _path;
	std::string _root;
	std::vector<std::string> _methods;
	std::vector<std::string> _index;
	bool _autoindex;
	std::string _returnUrl;
	int _returnCode;
	unsigned long _clientMaxBodySize;
	std::string _uploadStore;

	std::map<std::string, std::string> _cgiHandlers;

public:
	LocationConfig(const ServerConfig &serverConfig);
	~LocationConfig();
	LocationConfig(const LocationConfig &serverConfig);
	LocationConfig &operator=(const LocationConfig &o);

	// getters
	const std::string &getPath() const;
	const std::string &getRoot() const;
	const std::vector<std::string> &getMethods() const;
	const std::vector<std::string> &getIndex() const;
	bool getAutoindex() const;
	const std::string &getReturnUrl() const;
	int getReturnCode() const;
	unsigned long getClientMaxBodySize() const;
	const std::string &getUploadStore() const;
	const std::map<std::string, std::string> &getCgiHandlers() const;
	std::string getCgiPathForExt(const std::string &ext) const;

	// setter
	void setPath(const std::string &path, size_t line);
	void setRoot(const std::string &root, size_t line);
	void setMethods(const std::vector<std::string> &methods, size_t line);
	void setIndex(const std::vector<std::string> &index, size_t line);
	void setAutoIndex(const std::string &autoindex, size_t line);
	void setReturn(const std::vector<std::string> &params, size_t line);
	void setClientMaxBodySize(const std::string &clientMaxBodySize, size_t line);
	void setUploadStore(const std::string &path, size_t line);
	void setCgiHandel(const std::vector<std::string> &params, size_t line);
	// helpers
	bool hasCgiForExt(const std::string &ext) const;
	bool isMethodAllowed(const std::string &method) const;
	bool hasCgi() const;
	bool hasReturn() const;
	void validate(size_t line);
};

#endif
