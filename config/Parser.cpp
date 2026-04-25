#include "Lexer.hpp"
#include "Parser.hpp"
#include "locationConfig.hpp"
#include "ServerConfig.hpp"

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
	// cout << "Inside location" << endl;
	_index++;
};
void Config::parseDirective(ServerConfig &serverBlock)
{
	TokenType keyType = advance().getType();

	if (keyType == HOST)
		serverBlock.setHost(peek().getLexeme());
	else if (keyType == LISTEN)
		serverBlock.setPort(peek().getLexeme());
	else if (keyType == SERVER_NAME)
		serverBlock.setServerName(peek().getLexeme());
	else if (keyType == ROOT)
		serverBlock.setRoot(peek().getLexeme());
	else if (keyType == CLIENT_MAX_BODY_SIZE)
		serverBlock.setClientMaxBodySize(peek().getLexeme());
	// else if (keyType == ERROR_PAGE)
	// else if (keyType == ALLOW_METHODS)
	// else if (keyType == RETURN)
	// else if (keyType == AUTOINDEX)
	// else if (keyType == INDEX)
	// else if (keyType == UPLOAD_STORE)
	// else if (keyType == CGI_EXT)
	// else if (keyType == CGI_PATH)
	else
		parseError(peek(), "Unknown identifier"); // unknown identifier

	advance(); // skip the key
	if (!expect(SEMICOLON))
		parseError(peek(), "\';\' Expected"); // semicolon expected as end of line
}

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
