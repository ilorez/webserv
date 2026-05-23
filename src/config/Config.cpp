#include "../../includes/container.hpp"

// constuctor & destructor
Config::Config(std::vector<Token> Tokens)
{
	_status = 0;
	_index = 0;
	_Tokens = Tokens;

	this->Parser();

	if (this->_servers.size() == 0) // in case the config file is empty
	{
		ServerConfig serv;
		this->_servers.push_back(serv);
	}
};

Config::~Config() {};

// Copy constructor
Config::Config(const Config& other)
    : _Tokens(other._Tokens),
      _servers(other._servers),
      _status(other._status),
      _index(other._index)
{}

// operator=
Config& Config::operator=(const Config& other)
{
    if (this == &other)
        return *this;
    _Tokens = other._Tokens;
    _servers = other._servers;
    _status = other._status;
    _index = other._index;
    return *this;
}

// Getters and Setters
int Config::getStatus() const { return this->_status; }
std::vector<ServerConfig> &Config::getServers() { return _servers; }

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

// ? parsing
std::vector<std::string> Config::parseParams()
{
	std::vector<std::string> params;

	while (peek().getType() != SEMICOLON)
	{
		if (isAtEnd())
			errorMsg("Missing ';' after directive", peek().getLine());
		params.push_back(advance().getLexeme());
	}

	if (params.empty())
		errorMsg("directive requires at least one argument", peek().getLine());
	return (params);
}

// --------------------------- Server
void Config::parseServer()
{
	ServerConfig serverBlock;

	if (!expect(LEFT_BRACE))
		errorMsg("\'{\' Expected", peek().getLine() - 1);

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
		errorMsg("\'}\' Expected", peek().getLine() - 1);
	}
	serverBlock.validate(peek().getLine());
	this->_servers.push_back(serverBlock);
}

void Config::parseServerDirective(ServerConfig &serverBlock)
{
	size_t line = peek().getLine();
	TokenType keyType = advance().getType();
	std::string TokLexeme = peek().getLexeme();

	if (keyType == HOST)
		serverBlock.setHost(TokLexeme, line);
	else if (keyType == LISTEN)
		serverBlock.setPort(TokLexeme, line);
	else if (keyType == SERVER_NAME)
		serverBlock.setServerName(TokLexeme);
	else if (keyType == ROOT)
		serverBlock.setRoot(TokLexeme, line);
	else if (keyType == CLIENT_MAX_BODY_SIZE)
		serverBlock.setClientMaxBodySize(TokLexeme, line);
	else if (keyType == AUTOINDEX)
		serverBlock.setAutoIndex(TokLexeme, line);
	else if (keyType == ERROR_PAGE)
		serverBlock.setErrorPages(parseParams(), line);
	else if (keyType == INDEX)
		serverBlock.setIndex(parseParams(), line);
	else
		errorMsg("Unknown identifier", line);

	if (keyType == HOST || keyType == LISTEN || keyType == SERVER_NAME ||
		keyType == ROOT || keyType == CLIENT_MAX_BODY_SIZE || keyType == AUTOINDEX)
		advance();

	if (!expect(SEMICOLON))
		errorMsg("\';\' Expected", line);
}

// --------------------------- Location
void Config::parseLocation(ServerConfig &serverBlock)
{
	LocationConfig locationBlock(serverBlock);

	locationBlock.setPath(advance().getLexeme(), peek().getLine());
	if (!expect(LEFT_BRACE))
		errorMsg("\'{\' Expected", peek().getLine() - 1);

	while (!isAtEnd() && peek().getType() != RIGHT_BRACE)
		parseLocationDirective(locationBlock);

	if (!expect(RIGHT_BRACE))
		errorMsg("\'}\' Expected", peek().getLine() - 1);

	locationBlock.validate(peek().getLine());
	serverBlock.setLocation(locationBlock);
};
void Config::parseLocationDirective(LocationConfig &locationBlock)
{
	size_t line = peek().getLine();
	TokenType keyType = advance().getType();
	std::string TokLexeme = peek().getLexeme();

	if (keyType == ROOT)
		locationBlock.setRoot(TokLexeme, line);
	else if (keyType == CLIENT_MAX_BODY_SIZE)
		locationBlock.setClientMaxBodySize(TokLexeme, line);
	else if (keyType == AUTOINDEX)
		locationBlock.setAutoIndex(TokLexeme, line);
	else if (keyType == UPLOAD_STORE)
		locationBlock.setUploadStore(TokLexeme, line);
	else if(keyType == CGI_HANDEL)
		locationBlock.setCgiHandel(parseParams(), line);
	else if (keyType == INDEX)
		locationBlock.setIndex(parseParams(), line);
	else if (keyType == ALLOW_METHODS)
		locationBlock.setMethods(parseParams(), line);
	else if (keyType == RETURN)
		locationBlock.setReturn(parseParams(), line);
	else
		errorMsg("Unknown identifier", line);

	if (keyType == ROOT || keyType == CLIENT_MAX_BODY_SIZE || keyType == UPLOAD_STORE || keyType == AUTOINDEX)
		advance();

	if (!expect(SEMICOLON))
		errorMsg("\';\' Expected", line);
}

// --------------------------- Config
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
			errorMsg("Config file error", peek().getLine());
		}
	}
};
