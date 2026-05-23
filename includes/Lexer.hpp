#ifndef CONFIG_HPP
#define CONFIG_HPP

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

enum TokenType
{
	// structural
	LEFT_BRACE,
	RIGHT_BRACE,
	SEMICOLON,
	END_OF_FILE,

	// server-level keywords
	SERVER,
	HOST,
	LISTEN,
	ERROR_PAGE,
	CLIENT_MAX_BODY_SIZE,
	SERVER_NAME,

	// location-level keywords
	LOCATION,
	ALLOW_METHODS,
	RETURN,
	ROOT,
	AUTOINDEX,
	INDEX,
	UPLOAD_STORE,
	CGI_HANDEL,

	// values
	STRING,
	NUMBER,
};

class Token
{
private:
	TokenType _type;
	std::string _lexeme;
	size_t _line;

public:
	TokenType getType() const;
	std::string getLexeme() const;
	size_t getLine() const;

	Token(const TokenType &type, const std::string &lexeme, size_t line);
	~Token();
};

class Scanner
{
private:
	std::string _raw;
	std::vector<Token> Tokens;
	size_t _start, _current, _line;
	int _hadError;

public:
	// constructors
	Scanner(const std::string &raw);
	Scanner(const Scanner &other);
	Scanner &operator=(const Scanner &other);
	~Scanner();

	// helpers
	char peek();
	bool expect(char c);
	char advance();
	bool isAtEnd();
	void lexerError(char c);

	// parsing
	void addToken(const TokenType type);
	void addToken(const size_t len);
	void scanToken();
	void readWord();
	void readString();

	//   getters and setters
	int getHadError();
	std::vector<Token> getTokens();
};

#endif
