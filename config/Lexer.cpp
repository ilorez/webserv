#include "Lexer.hpp"

using namespace std;

// Constructor
Scanner::Scanner(const std::string &raw)
{
	_start = _current = 0;
	_line = 1;
	_hadError = 0;
	this->_raw = raw;

	while (!isAtEnd())
	{
		_start = _current;
		scanToken();
	}
	addToken(END_OF_FILE);
}
Scanner::Scanner(const Scanner &other) : _raw(other._raw)
{
}

Scanner &Scanner::operator=(const Scanner &other)
{
	if (this != &other)
		this->_raw = other._raw;
	return (*this);
}

Scanner::~Scanner() {}

// ? Getters and Setters
int Scanner::getHadError()
{
	return (this->_hadError);
}

TokenType Token::getType() const { return this->_type; }
std::string Token::getLexeme() const { return this->_lexeme; }
size_t Token::getLine() const { return this->_line; }

std::vector<Token> Scanner::getTokens()
{
	return (this->Tokens);
}

// helpers
char Scanner::advance()
{
	return (this->_raw[_current++]);
}

bool Scanner::expect(char c)
{
	if (isAtEnd())
		return false;
	if (c != this->_raw[_current])
		return false;

	_current++;
	return (true);
}

void Scanner::lexerError(char c)
{
	_hadError = 1;
	std::cerr << "Error: " << "Unexpected Character " << "\"" << c << "\"" << " in _line " << _line << std::endl;
}

bool Scanner::isAtEnd()
{
	return _current < this->_raw.size() ? false : true;
}

char Scanner::peek()
{
	return (this->_raw[_current]);
}

// parsing
void Scanner::readWord()
{
	char c;

	while (!isAtEnd())
	{
		c = peek();
		if (isalnum(c) || c == '.' || c == '/' || c == '_' || c == '-')
			advance();
		else
			break;
	}

	c = peek();
	if (!isAtEnd() && c != '{' && c != ' ' && c != ';' && c != '\n' && c != '\t' && c != '\r')
	{
		lexerError(c);
		return;
	}
	addToken(_current - _start);
}

void Scanner::readString()
{
	while (!isAtEnd() && peek() != '"')
	{
		if (peek() == '\n')
			_line++;
		advance();
	}

	if (isAtEnd())
	{
		_hadError = 1;
		cerr << "Error: Unterminated string at line " << _line << endl;
		return;
	}

	advance();

	std::string literal = _raw.substr(_start + 1, _current - _start - 2);
	Token newToken(STRING, literal, _line);
	Tokens.push_back(newToken);
}

void Scanner::scanToken()
{
	char c = advance();

	switch (c)
	{
	case '{':
		addToken(LEFT_BRACE);
		break;
	case '}':
		addToken(RIGHT_BRACE);
		break;
	case ';':
		addToken(SEMICOLON);
		break;
	case '#':
		while (!isAtEnd() && this->_raw[_current] != '\n')
			_current++;
		break;
	case '/':
		if (expect('/'))
		{
			while (!isAtEnd() && this->_raw[_current] != '\n')
				_current++;
			break;
		}
		_current--;
		readWord();
		break;
	case '"':
		readString();
		break;
	case '\n':
		_line++;
		break;
	case ' ':
	case '\r':
	case '\t':
		break;
	default:
		readWord();
		break;
	}
}

void Scanner::addToken(const TokenType type)
{
	Token newToken(type, "", _line);

	this->Tokens.push_back(newToken);
}

void Scanner::addToken(const size_t len)
{
	TokenType type;
	std::string literal = this->_raw.substr(_start, len);

	if (literal == "server")
		type = SERVER;
	else if (literal == "host")
		type = HOST;
	else if (literal == "listen")
		type = LISTEN;
	else if (literal == "server_name")
		type = SERVER_NAME;
	else if (literal == "root")
		type = ROOT;
	else if (literal == "index")
		type = INDEX;
	else if (literal == "error_page")
		type = ERROR_PAGE;
	else if (literal == "client_max_body_size")
		type = CLIENT_MAX_BODY_SIZE;
	else if (literal == "location")
		type = LOCATION;
	else if (literal == "autoindex")
		type = AUTOINDEX;
	else if (literal == "allow_methods")
		type = ALLOW_METHODS;
	else if (literal == "return")
		type = RETURN;
	else if (literal == "cgi_path")
		type = CGI_PATH;
	else if (literal == "cgi_ext")
		type = CGI_EXT;
	else if (literal == "upload_store")
		type = UPLOAD_STORE;
	else if (literal.find_first_not_of("0123456789") == string::npos)
		type = NUMBER;
	else
		type = STRING;

	Token newToken(type, literal, _line);
	this->Tokens.push_back(newToken);
}
