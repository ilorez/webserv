
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

class LocationConfig
{
private:
	std::string _path;
	std::string _root;
	vector<string> _methods;
	std::vector<std::string> _index;
	bool _autoindex;

	// return

	unsigned long clientMaxBodySize;

public:
	LocationConfig(const ServerConfig &ServerConfig) {};

	~LocationConfig() {};
};
#endif