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
#include "Lexer.hpp"
#include "serverConfig.hpp"

class LocationConfig;

class Config
{
private:
	std::vector<Token> _Tokens;
	std::vector<ServerConfig> _servers;
	int _status;
	size_t _index;

public:
	// constructors
	Config(std::vector<Token> Tokens);
	~Config();
  Config(const Config& other);
  Config& operator=(const Config& other);

	// parsing
	void Parser();
	void parseServer();
	void parseLocation(ServerConfig &serverBlock);
	void parseServerDirective(ServerConfig &serverBlock);
	void parseLocationDirective(LocationConfig &locationBlock);

	std::vector<std::string> parseParams();

	// helpers
	bool expect(TokenType type);
	Token advance();
	Token peek();
	bool isAtEnd();

	// getters and setters
	int getStatus() const;
  std::vector<ServerConfig> &getServers();
};

#endif
