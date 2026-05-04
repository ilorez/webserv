#include "Lexer.hpp"
#include "Config.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"
#include "utils.hpp"

//? constructor & destructor
ServerConfig::ServerConfig()
{
	this->_host = "127.0.0.1";
	this->_port = 8080;
	this->_serverName = "localhost";
	this->_root = "./html";				// ./html/ . like nginx default
	this->_clientMaxBodySize = 1048576; // 1mg, like nginx default
	this->_autoindex = false;
	this->_index = {"index.html"};
};

ServerConfig::~ServerConfig() {};

//? getters
const std::string &ServerConfig::getHost() const { return this->_host; }
uint16_t ServerConfig::getPort() const { return this->_port; }
const std::string &ServerConfig::getServerName() const { return this->_serverName; }
const std::string &ServerConfig::getRoot() const { return this->_root; }
unsigned long ServerConfig::getClientMaxBodySize() const { return this->_clientMaxBodySize; }
const std::vector<std::string> &ServerConfig::getIndex() const { return this->_index; }
bool ServerConfig::getAutoIndex() const { return this->_autoindex; }
const std::map<int, std::string> &ServerConfig::getErrorPages() const { return this->_errorPage; }
const std::vector<LocationConfig> &ServerConfig::getLocations() const { return this->_locations; }

//? helpers

// ? setters
void ServerConfig::setClientMaxBodySize(const std::string &clientMaxBodySize, size_t line) // ! i might need to check for overflow
{
	uint64_t num = 0;
	size_t i = 0;

	if (clientMaxBodySize.empty())
		errorMsg("Invalid client_max_body_size", line);

	if (clientMaxBodySize.find_first_not_of("0") == std::string::npos)
	{
		this->_clientMaxBodySize = 0;
		return;
	}

	for (; i < clientMaxBodySize.size() && isdigit(clientMaxBodySize[i]); i++)
		num = num * 10 + (clientMaxBodySize[i] - '0');

	if (i < clientMaxBodySize.size())
	{
		if (i != clientMaxBodySize.size() - 1)
			errorMsg("Invalid client_max_body_size", line);

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
			errorMsg("Invalid client_max_body_size suffix", line);
		}
	}

	if (num == 0)
		errorMsg("Invalid client_max_body_size", line);

	this->_clientMaxBodySize = num;
}

void ServerConfig::setRoot(const std::string &root, size_t line)
{
	if (!isValidPath(root))
		errorMsg("Invalid root path", line);

	this->_root = root;
}

void ServerConfig::setServerName(const std::string &serverName) { this->_serverName = serverName; }

void ServerConfig::setHost(const std::string &host, size_t line)
{
	if (host.empty())
		errorMsg("Invalid Host", line);

	if (host == "localhost")
	{
		this->_host = host;
		return;
	}

	std::istringstream ss(host);
	std::string octet;
	int count = 0;

	while (std::getline(ss, octet, '.'))
	{
		count++;
		if (octet.empty() || octet.size() > 3)
			errorMsg("Invalid Host", line);

		if (octet.find_first_not_of("0123456789") != std::string::npos)
			errorMsg("Invalid Host", line);

		int num = atoi(octet.c_str());
		if (num < 0 || num > 255)
			errorMsg("Invalid Host", line);
	}

	if (count != 4)
		errorMsg("Invalid Host", line);

	this->_host = host;
}

void ServerConfig::setPort(const std::string &port, size_t line)
{
	if (port.find_first_not_of("0123456789") != std::string::npos)
		errorMsg("Invalid Port", line);

	long num = atol(port.c_str());
	if (num < 1024 || num > 65535)
		errorMsg("Invalid Port", line);

	this->_port = (uint16_t)num;
}

void ServerConfig::setErrorPages(const std::vector<std::string> &errorPage, size_t line)
{
	std::vector<int> vec;

	for (size_t i = 0; i < errorPage.size(); i++)
	{
		if (!isValidStatusCode(errorPage[i]))
		{
			break;
		}
		vec.push_back(atoi(errorPage[i].c_str()));
	}

	if (vec.empty() || vec.size() != errorPage.size() - 1)
		errorMsg("Invalid error_page Arguments", line);

	std::string path = errorPage[vec.size()];
	if (!isValidPath(path))
		errorMsg("Invalid error_page path", line);
	for (size_t i = 0; i < vec.size(); i++)
	{
		_errorPage.insert(std::make_pair(vec[i], path));
	}
}

void ServerConfig::setIndex(const std::vector<std::string> &index, size_t line)
{
	std::string file;
	this->_index.clear();

	for (size_t i = 0; i < index.size(); i++)
	{
		file = index[i];
		if (file.empty())
			errorMsg("File cannot be empty.", line);
		if (file.find_first_of("*?[]()!<") != std::string::npos)
			errorMsg("Invalid path." + file, line);
		this->_index.push_back(file);
	}
}

void ServerConfig::setAutoIndex(const std::string &index, size_t line)
{
	if (index == "on")
		this->_autoindex = true;
	else if (index == "off")
		this->_autoindex = false;
	else
		errorMsg("Invalid autoIndex value.", line);
}

void ServerConfig::setLocation(const LocationConfig &locationBlock)
{
	this->_locations.push_back(locationBlock);
}
