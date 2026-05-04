#include "utils.hpp"

bool isValidPath(const std::string &path)
{
	if (path.empty() || path.find_first_of("*?[]()!<") != std::string::npos)
		return false;

	return true;
}

bool isValidStatusCode(const std::string &str)
{
	int num;

	if (str.size() != 3 || str.find_first_not_of("0123456789") != std::string::npos)
		return false;

	num = atoi(str.c_str());
	return num < 100 || num > 599 ? false : true;
}

void errorMsg(const std::string &msg, size_t line)
{
	std::ostringstream oss;
	std::string fullMsg;

	oss << "ERROR: " << msg << ". Line: " << line;
	fullMsg = oss.str();

	throw std::runtime_error(fullMsg);
}