#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

// constructors & destructors
LocationConfig::LocationConfig(const ServerConfig &serverConfig)
	: _path(""), _root(serverConfig.getRoot()), _index(serverConfig.getIndex()), _autoindex(serverConfig.getAutoIndex())
	  // , _returnUrl("")
	  // , _returnCode(0)
	  ,
	  _clientMaxBodySize(serverConfig.getClientMaxBodySize())
{
}

LocationConfig::~LocationConfig() {};

// getters
const std::string &LocationConfig::getPath() const { return _path; }
const std::string &LocationConfig::getRoot() const { return _root; }
const std::vector<std::string> &LocationConfig::getMethods() const { return _methods; }
const std::vector<std::string> &LocationConfig::getIndex() const { return _index; }
bool LocationConfig::getAutoindex() const { return _autoindex; }
// const std::string &LocationConfig::getReturnUrl() const { return _returnUrl; }
// int LocationConfig::getReturnCode() const { return _returnCode; }
unsigned long LocationConfig::getClientMaxBodySize() const { return _clientMaxBodySize; }

// setters
void LocationConfig::setPath(const std::string &path)
{
	if (!isValidPath(path))
		throw logic_error("Expect a valid path after 'location'"); // !

	this->_path = path;
}
void LocationConfig::setRoot(const std::string &root)
{

	if (!isValidPath(root))
		throw logic_error("Invalid root path"); // !

	this->_root = root;
}
void LocationConfig::setMethods(const std::vector<std::string> &methods)
{
	this->_methods.clear();

	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE")
			throw std::invalid_argument("Invalid HTTP method: " + methods[i]);

		for (size_t j = 0; j < this->_methods.size(); j++)
		{
			if (this->_methods[j] == methods[i])
				throw std::invalid_argument("Duplicate HTTP method: " + methods[i]);
		}

		this->_methods.push_back(methods[i]);
	}
}
void LocationConfig::setIndex(const std::vector<std::string> &index)
{
	std::string file;
	this->_index.clear();

	for (size_t i = 0; i < index.size(); i++)
	{
		file = index[i];
		if (file.empty())
			throw logic_error("File cannot be empty.");
		if (file.find_first_of("*?[]()!<") != string::npos)
			throw logic_error("Invalid path." + file);
		this->_index.push_back(file);
	}
}
void LocationConfig::setAutoIndex(const std::string autoindex)
{
	if (autoindex == "on")
		this->_autoindex = true;
	else if (autoindex == "off")
		this->_autoindex = false;
	else
		throw logic_error("Invalid autoIndex value.");
}
// void LocationConfig::setReturnUrl(const std::string &url) { _returnUrl = url; }
// void LocationConfig::setReturnCode(int code) { _returnCode = code; }
void LocationConfig::setClientMaxBodySize(const string &clientMaxBodySize) // ! i might need to check for overflow
{
	uint64_t num = 0;
	size_t i = 0;

	if (clientMaxBodySize.empty())
		throw logic_error("Invalid client_max_body_size");

	if (clientMaxBodySize.find_first_not_of("0") == string::npos)
	{
		this->_clientMaxBodySize = 0;
		return;
	}

	for (; i < clientMaxBodySize.size() && isdigit(clientMaxBodySize[i]); i++)
		num = num * 10 + (clientMaxBodySize[i] - '0');

	if (i < clientMaxBodySize.size())
	{
		if (i != clientMaxBodySize.size() - 1)
			throw logic_error("Invalid client_max_body_size");

		switch (toupper(clientMaxBodySize[i]))
		{
		case 'B':
			break;
		case 'K':
			num *= 1024;
			break;
		case 'M':
			num *= 1024 * 1024;
			break;
		case 'G':
			num *= 1024 * 1024 * 1024;
			break;
		default:
			throw logic_error("Invalid client_max_body_size suffix");
		}
	}

	if (num == 0)
		throw logic_error("Invalid client_max_body_size");

	this->_clientMaxBodySize = num;
}

// helpers
bool LocationConfig::isMethodAllowed(const std::string &method) const
{
	return std::find(_methods.begin(), _methods.end(), method) != _methods.end();
}
bool LocationConfig::isValidPath(const std::string &path)
{
	if (path.empty() || path.find_first_of("*?[]()!<") != string::npos)
		return false;

	return true;
}
bool LocationConfig::IsvalidStatusCode(const std::string &str)
{
	int num;

	if (str.size() != 3 || str.find_first_not_of("0123456789") != string::npos)
		return false;

	num = atoi(str.c_str());
	return num < 100 || num > 599 ? false : true;
}
// parsing