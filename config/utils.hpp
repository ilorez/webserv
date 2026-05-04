
#ifndef UTILS_HPP
#define UTILS_HPP

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
#include "sstream"

bool isValidPath(const std::string &path);
bool isValidStatusCode(const std::string &str);
void errorMsg(const std::string &msg, size_t line);

#endif