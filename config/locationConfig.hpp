
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
	vector<string> _methods;
	string index;

public:
	LocationConfig() {};
	~LocationConfig() {};
};
#endif