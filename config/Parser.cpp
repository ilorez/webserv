#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

// constuctor & destructor
Config::Config(std::vector<Token> &Tokens)
{
	_status = 0;
	_index = 0;
	_Tokens = Tokens;

	this->Parser();
};

Config::~Config() {};

// Getters and Setters
int Config::getStatus() const { return this->_status; }

// helpers
bool Config::expect(const TokenType type)
{
	if (type == END_OF_FILE)
		return false;
	if (type != _Tokens[_index].getType())
		return false;

	_index++;
	return (true);
}
Token Config::advance()
{
	return (this->_Tokens[_index++]);
}
Token Config::peek()
{
	return (this->_Tokens[_index]);
}
bool Config::isAtEnd()
{
	return (peek().getType() == END_OF_FILE ? true : false);
}
static void parseError(const Token &token, const std::string &msg)
{
	cerr << msg << ". Line: " << token.getLine() << endl;
	exit(1);
}

// parsing
std::vector<std::string> Config::parseParams()
{
	std::vector<std::string> params;

	while (peek().getType() != SEMICOLON)
	{
		if (isAtEnd())
			throw std::runtime_error("Missing ';' after directive");
		params.push_back(advance().getLexeme());
	}

	if (params.empty())
		throw std::runtime_error("directive requires at least one argument");
	return (params);
}

void Config::parseServer()
{
	ServerConfig serverBlock;

	if (!expect(LEFT_BRACE))
		parseError(peek(), "\'{\' Expected");

	while (!isAtEnd() && peek().getType() != RIGHT_BRACE)
	{
		if (peek().getType() == LOCATION)
		{
			advance();
			parseLocation(serverBlock);
		}
		else
		{
			parseServerDirective(serverBlock);
		}
	}
	if (!expect(RIGHT_BRACE))
	{
		parseError(peek(), "\'}\' Expected");
	}
	this->_servers.push_back(serverBlock);
}

void Config::parseLocation(ServerConfig &serverBlock)
{
	LocationConfig locationBlock(serverBlock);

	locationBlock.setPath(advance().getLexeme());
	if (!expect(LEFT_BRACE))
		parseError(peek(), "\'{\' Expected");

	while (!isAtEnd() && peek().getType() != RIGHT_BRACE)
	{
		parseLocationDirective(locationBlock);
	}
	if (!expect(RIGHT_BRACE))
	{
		parseError(peek(), "\'}\' Expected");
	}
	serverBlock.setLocation(locationBlock);
};
void Config::parseServerDirective(ServerConfig &serverBlock)
{
	TokenType keyType = advance().getType();
	std::string TokLexeme = peek().getLexeme();

	if (keyType == HOST)
		serverBlock.setHost(TokLexeme);
	else if (keyType == LISTEN)
		serverBlock.setPort(TokLexeme);
	else if (keyType == SERVER_NAME)
		serverBlock.setServerName(TokLexeme);
	else if (keyType == ROOT)
		serverBlock.setRoot(TokLexeme);
	else if (keyType == CLIENT_MAX_BODY_SIZE)
		serverBlock.setClientMaxBodySize(TokLexeme);
	else if (keyType == AUTOINDEX)
		serverBlock.setAutoIndex(TokLexeme);
	else if (keyType == ERROR_PAGE)
		serverBlock.setErrorPages(parseParams());
	else if (keyType == INDEX)
		serverBlock.setIndex(parseParams());
	else
		parseError(peek(), "Unknown identifier");

	if (keyType == HOST || keyType == LISTEN || keyType == SERVER_NAME ||
		keyType == ROOT || keyType == CLIENT_MAX_BODY_SIZE || keyType == AUTOINDEX)
		advance();

	if (!expect(SEMICOLON))
		parseError(peek(), "\';\' Expected");
}

void Config::parseLocationDirective(LocationConfig &locationBlock)
{
	TokenType keyType = advance().getType();
	std::string TokLexeme = peek().getLexeme();

	if (keyType == ROOT)
		locationBlock.setRoot(TokLexeme);
	else if (keyType == CLIENT_MAX_BODY_SIZE)
		locationBlock.setClientMaxBodySize(TokLexeme);
	else if (keyType == AUTOINDEX)
		locationBlock.setAutoIndex(TokLexeme);
	else if (keyType == INDEX)
		locationBlock.setIndex(parseParams());
	else if (keyType == ALLOW_METHODS)
		locationBlock.setMethods(parseParams());
	else
		parseError(peek(), "Unknown identifier");

	if (keyType == ROOT || keyType == CLIENT_MAX_BODY_SIZE || keyType == AUTOINDEX)
		advance();

	if (!expect(SEMICOLON))
		parseError(peek(), "\';\' Expected");
}
void Config::Parser()
{
	while (!isAtEnd())
	{
		if (peek().getType() == SERVER)
		{
			advance();
			parseServer();
			// printServerData(this->_servers);
		}
		else
		{
			parseError(peek(), "Config file Error");
		}
	}
};
