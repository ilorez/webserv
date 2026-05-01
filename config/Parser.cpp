#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

std::vector<std::string> Config::parseParams()
{
	std::vector<std::string> params;
	std::string param = advance().getLexeme();

	while (param != ";")
	{
		if (isAtEnd())
		{
			cout << peek().getLine() << endl;
			throw std::runtime_error("Missing ';' after directive");
		}
		params.push_back(param);
		param = advance().getLexeme();
	}

	if (params.empty())
		throw std::runtime_error("directive requires at least one argument");
	return (params);
}

// Getters and Setters
int Config::getStatus() const { return this->_status; }

Config::Config(std::vector<Token> &Tokens)
{
	_status = 0;
	_index = 0;
	_Tokens = Tokens;

	this->Parser();
};

// helper methods
static void parseError(const Token &token, const std::string &msg) // todo: write a better error handling approach
{
	cerr << msg << ". Line: " << token.getLine() << endl;
	exit(1);
}

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
			parseDirective(serverBlock);
		}
	}
	if (!expect(RIGHT_BRACE))
	{
		parseError(peek(), "\'}\' Expected");
	}
}

void Config::parseLocation(ServerConfig &serverBlock)
{
	LocationConfig locationBlock;
	// cout << "Inside ~location" << endl;
	_index++;
};
void Config::parseDirective(ServerConfig &serverBlock)
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
		serverBlock.setClientMaxBodySize(TokLexeme); // an9adha dbbb
	else if (keyType == AUTOINDEX)
		serverBlock.setAutoIndex(TokLexeme);
	else if (keyType == ERROR_PAGE)
		serverBlock.setErrorPages(parseParams());
	else if (keyType == INDEX)
		serverBlock.setIndex(parseParams());
	else
		parseError(peek(), "Unknown identifier"); // unknown identifier

	if (keyType == HOST || keyType == LISTEN || keyType == SERVER_NAME || keyType == ROOT || keyType == CLIENT_MAX_BODY_SIZE || keyType == AUTOINDEX)
		advance();
	if (!expect(SEMICOLON))
		parseError(peek(), "\';\' Expected"); // semicolon expected as end of line
}
// else if (keyType == ALLOW_METHODS)
// 	parseAllowedMethods(serverBlock);

void Config::Parser()
{
	while (!isAtEnd())
	{
		if (peek().getType() == SERVER)
		{
			advance();
			parseServer();
		}
		else
		{
			parseError(peek(), "Config file Error");
		}
	}
};

Config::~Config() {};
