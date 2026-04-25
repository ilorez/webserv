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
#include "Lexer.hpp"
#include "locationConfig.hpp"


using namespace std;


class ServerConfig
{
private:
	std::string _host;
	unsigned int _port;
	std::string _serverName;
	std::string _root;
	size_t _clientMaxBodySize;
	std::string _index;
	vector<LocationConfig> _locations;

public:
	ServerConfig();
	~ServerConfig();

	// getters and setters
	void setHost(const string &host);
	void setPort(const std::string &port);
	void setServerName(const string &serverName);
	void setRoot(const string &root);
	void setClientMaxBodySize(const string &clientMaxBodySize);
	// void setIndex(const string &index) { this->_index = index; }
};

#endif