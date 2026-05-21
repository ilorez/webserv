#include "../../includes/container.hpp"

// constructors & destructors
LocationConfig::LocationConfig(const ServerConfig &serverConfig)
	: _path(""),
	  _root(serverConfig.getRoot()),
	  _index(serverConfig.getIndex()),
	  _autoindex(serverConfig.getAutoIndex()),
	  _returnUrl(""),
	  _returnCode(0),
	  _clientMaxBodySize(serverConfig.getClientMaxBodySize()),
	  _cgiExtension(""),
	  _cgiPath(""),
	  _uploadStore("../../uploads/")
{
	_methods.push_back("GET");
	_methods.push_back("POST");
	_methods.push_back("DELETE");
}
LocationConfig::LocationConfig(const LocationConfig& copy)
{
  *this = copy;
}

LocationConfig& LocationConfig::operator=(const LocationConfig& o)
{
  if (this != &o)
  {
    this->_path  = o._path;
    this->_root = o._root;
    this->_methods = o._methods;
    this->_index = o._index;
    this->_autoindex = o._autoindex;
    this->_returnUrl = o._returnUrl;
    this->_returnCode = o._returnCode;
    this->_clientMaxBodySize = o._clientMaxBodySize;
    this->_cgiExtension = o._cgiExtension;
    this->_cgiPath = o._cgiPath;
    this->_uploadStore = o._uploadStore;
  }
  return *this;
}
LocationConfig::~LocationConfig() {};

// getters
const std::string &LocationConfig::getPath() const { return _path; }
const std::string &LocationConfig::getRoot() const { return _root; }
const std::vector<std::string> &LocationConfig::getMethods() const { return _methods; }
const std::vector<std::string> &LocationConfig::getIndex() const { return _index; }
bool LocationConfig::getAutoindex() const { return _autoindex; }
const std::string &LocationConfig::getReturnUrl() const { return _returnUrl; }
int LocationConfig::getReturnCode() const { return _returnCode; }
unsigned long LocationConfig::getClientMaxBodySize() const { return _clientMaxBodySize; }
const std::string &LocationConfig::getCgiExt() const { return _cgiExtension; }
const std::string &LocationConfig::getCgiPath() const { return _cgiPath; }
const std::string &LocationConfig::getUploadStore() const { return _uploadStore; }
// setters
void LocationConfig::setPath(const std::string &path, size_t line)
{
	if (!isValidPath(path))
		errorMsg("Expect a valid path after 'location'", line); // !

	this->_path = path;
}
void LocationConfig::setRoot(const std::string &root, size_t line)
{

	if (!isValidPath(root))
		errorMsg("Invalid root path", line); // !

	this->_root = root;
}
void LocationConfig::setMethods(const std::vector<std::string> &methods, size_t line)
{
	this->_methods.clear();

	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE")
			errorMsg("Invalid HTTP method: " + methods[i], line);

		for (size_t j = 0; j < this->_methods.size(); j++)
		{
			if (this->_methods[j] == methods[i])
				errorMsg("Duplicate HTTP method: " + methods[i], line);
		}

		this->_methods.push_back(methods[i]);
	}
}
void LocationConfig::setIndex(const std::vector<std::string> &index, size_t line)
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
void LocationConfig::setAutoIndex(const std::string &autoindex, size_t line)
{
	if (autoindex == "on")
		this->_autoindex = true;
	else if (autoindex == "off")
		this->_autoindex = false;
	else
		errorMsg("Invalid autoIndex value.", line);
}
void LocationConfig::setReturn(const std::vector<std::string> &params, size_t line)
{
	if (params.size() > 2)
		errorMsg("'return' directive takes at most 2 arguments", line);

	if (params.size() == 1 && !params[0].empty() && (params[0][0] == '/' || params[0].find("http") == 0))
	{
		_returnCode = 302;
		_returnUrl = params[0];
		return;
	}

	if (!isValidStatusCode(params[0]))
		errorMsg("'return' directive has invalid status code: " + params[0], line);
	int code = atoi(params[0].c_str());
	_returnCode = code;

	if (params.size() == 2)
		_returnUrl = params[1];
	else
	{
		if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308)
			errorMsg("redirect code " + params[0] + " requires a URL", line);
	}
}
void LocationConfig::setClientMaxBodySize(const std::string &clientMaxBodySize, size_t line) // ! i might need to check for overflow
{
	unsigned int num = 0;
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
void LocationConfig::setUploadStore(const std::string &path, size_t line)
{
	if (!isValidPath(path))
		errorMsg("Invalid upload_store argument", line);

	_uploadStore = path;
}

void LocationConfig::setCgiExt(const std::string &ext, size_t line)
{
	if (!(ext == ".py" || ext == ".sh")) // ! i will add the rest of cgi's as needed
		errorMsg("Invalid cgi_ext, e.g. '.py', '.sh'", line);

	_cgiExtension = ext;
}

void LocationConfig::setCgiPath(const std::string &path, size_t line)
{
	if (!isValidPath(path))
		errorMsg("Invalid cgi_path", line);
	_cgiPath = path;
}

// helpers
bool LocationConfig::isMethodAllowed(const std::string &method) const
{
	return (std::find(_methods.begin(), _methods.end(), method) != _methods.end());
}

bool LocationConfig::hasCgi() const
{
	return (!_cgiExtension.empty() && !_cgiPath.empty());
}

bool LocationConfig::hasReturn() const
{
	return (_returnCode != 0);
}
// parsing
