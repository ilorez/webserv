#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->_host = "127.0.0.1";
	this->_port = 8080;
	this->_serverName = "localhost";
	this->_root = "html";				// ./html/ . like nginx default
	this->_clientMaxBodySize = 1048576; // 1mg, like nginx default
	this->_autoindex = false;
	this->_index = {"index.html"};
};

ServerConfig::~ServerConfig() {};

// ? methods
void ServerConfig::setClientMaxBodySize(const string &clientMaxBodySize) // ! i might need to check for overflow
{
	uint64_t num = 0;
	size_t i = 0;

	if (clientMaxBodySize.find_first_not_of("0") == string::npos)
	{
		this->_clientMaxBodySize = 0;
		return;
	}

	if (clientMaxBodySize.empty())
		throw logic_error("Invalid client_max_body_size");

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

void ServerConfig::setRoot(const string &root)
{
	if (!isValidPath(root))
		throw logic_error("Invalid root path");

	this->_root = root;
}

void ServerConfig::setServerName(const string &serverName) { this->_serverName = serverName; }

void ServerConfig::setHost(const string &host)
{
	if (host.empty())
		throw logic_error("Invalid Host");

	std::istringstream ss(host);
	std::string octet;
	int count = 0;

	while (std::getline(ss, octet, '.'))
	{
		count++;
		if (octet.empty() || octet.size() > 3)
			throw logic_error("Invalid Host");

		if (octet.find_first_not_of("0123456789") != string::npos)
			throw logic_error("Invalid Host");

		int num = atoi(octet.c_str());
		if (num < 0 || num > 255)
			throw logic_error("Invalid Host");
	}

	if (count != 4)
		throw logic_error("Invalid Host");

	this->_host = host;
}

void ServerConfig::setPort(const std::string &port)
{
	if (port.find_first_not_of("0123456789") != std::string::npos)
		throw logic_error("Invalid Port");

	int num = atoi(port.c_str());
	if (num < 1024 || num > 65535)
		throw logic_error("Invalid Port");

	this->_port = (uint16_t)num;
}

void ServerConfig::setErrorPages(const std::vector<std::string> &errorPage)
{
	_errorPage.clear();
	std::vector<int> vec;

	for (size_t i = 0; i < errorPage.size(); i++)
	{
		if (!IsvalidStatusCode(errorPage[i]))
		{
			break;
		}
		vec.push_back(atoi(errorPage[i].c_str()));
	}

	if (vec.empty() || vec.size() != errorPage.size() - 1)
		throw runtime_error("Invalid error_page Arguments");

	std::string path = errorPage[vec.size()];
	if (!isValidPath(path))
		throw logic_error("Invalid error_page path");
	for (size_t i = 0; i < vec.size(); i++)
	{
		_errorPage.insert(std::make_pair(vec[i], path));
	}
}

void ServerConfig::setIndex(const std::vector<std::string> &index)
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

void ServerConfig::setAutoIndex(const std::string &index)
{
	if (index == "on")
		this->_autoindex = true;
	else if (index == "off")
		this->_autoindex = false;
	else
		throw logic_error("Invalid autoIndex value.");
}

void ServerConfig::setLocation(const LocationConfig &locationBlock)
{
	this->_locations.push_back(locationBlock);
}

const std::string &ServerConfig::getHost() const { return this->_host; }
uint16_t ServerConfig::getPort() const { return this->_port; }
const std::string &ServerConfig::getServerName() const { return this->_serverName; }
const std::string &ServerConfig::getRoot() const { return this->_root; }
unsigned long ServerConfig::getClientMaxBodySize() const { return this->_clientMaxBodySize; }
const std::vector<std::string> &ServerConfig::getIndex() const { return this->_index; }
bool ServerConfig::getAutoIndex() const { return this->_autoindex; }
const std::map<int, std::string> &ServerConfig::getErrorPages() const { return this->_errorPage; }
const std::vector<LocationConfig> &ServerConfig::getLocations() const { return this->_locations; }

// helpers

bool ServerConfig::isValidPath(const std::string &path)
{
	if (path.empty() || path.find_first_of("*?[]()!<") != string::npos)
		return false;

	return true;
}

bool ServerConfig::IsvalidStatusCode(const std::string &str)
{
	int num;

	if (str.size() != 3 || str.find_first_not_of("0123456789") != string::npos)
		return false;

	num = atoi(str.c_str());
	return num < 100 || num > 599 ? false : true;
}