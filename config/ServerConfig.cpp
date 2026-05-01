#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	this->_host = "127.0.0.1";
	this->_port = 8080;
	this->_clientMaxBodySize = 0;
};

ServerConfig::~ServerConfig() {};

// ? methods
void ServerConfig::setClientMaxBodySize(const string &clientMaxBodySize) // todo: ask friends about the max body size that we would support
{
	if (clientMaxBodySize.find_first_not_of("0123456789") != string::npos)
	{
		throw logic_error("Invalid ClientMaxBodySize");
	}
	int num = atoi(clientMaxBodySize.c_str());
	this->_clientMaxBodySize = num;
}

void ServerConfig::setRoot(const string &root)
{
	if (root.find_first_of("*?[]()!<") != string::npos)
		throw logic_error("Invalid path.");

	this->_root = root;
}

void ServerConfig::setServerName(const string &serverName) { this->_serverName = serverName; }

void ServerConfig::setHost(const string &host) // todo: needs a better parsing
{
	if (host.empty() || host.find_first_not_of("0123456789.") != string::npos || std::count(host.begin(), host.end(), '.') != 3)
	{
		throw logic_error("Invalid Host");
	}
	this->_host = host;
}

void ServerConfig::setPort(const std::string &port) // todo: needs a better parsing
{
	int num;

	num = atoi(port.c_str());
	if (num <= 0 || num > 65535) //? should make into acount the system and reserved port
		throw logic_error("Invalid Port");

	this->_port = num;
}

void ServerConfig::setErrorPages(const std::vector<std::string> &errorPage)
{
}

void ServerConfig::setIndex(const std::vector<std::string> &index)
{
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