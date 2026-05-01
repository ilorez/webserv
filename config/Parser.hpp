#ifndef PARSER_HPP
#define PARSER_HPP

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
#include "ServerConfig.hpp"
#include "locationConfig.hpp"

using namespace std;

class Config
{
private:
	std::vector<Token> _Tokens;
	std::vector<ServerConfig> _servers;
	int _status;
	size_t _index;

public:
	// constructors
	Config(std::vector<Token> &Tokens);
	~Config();

	// methods
	void Parser();
	void parseServer();
	void parseLocation(ServerConfig &serverBlock);
	void parseDirective(ServerConfig &serverBlock);

	std::vector<std::string> parseParams();
	

	bool expect(TokenType type);
	Token advance();
	Token peek();
	bool isAtEnd();

	// getters and setters
	int getStatus() const;
};

#endif