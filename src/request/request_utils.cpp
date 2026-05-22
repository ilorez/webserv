
#include "../../includes/container.hpp"

// ? member functions

bool Request::isMethodAllowed(const std::string &method)
{
	const LocationConfig *loc = getMatchedLocation();

	if (loc && !loc->isMethodAllowed(method))
	{
		return false;
	}
	return true;
}

static void initCommaHeaders(std::set<std::string> &commaHeaders)
{
	commaHeaders.insert("accept");
	commaHeaders.insert("accept-charset");
	commaHeaders.insert("accept-encoding");
	commaHeaders.insert("accept-language");
	commaHeaders.insert("accept-ranges");
	commaHeaders.insert("allow");
	commaHeaders.insert("cache-control");
	commaHeaders.insert("connection");
	commaHeaders.insert("content-encoding");
	commaHeaders.insert("content-language");
	commaHeaders.insert("if-match");
	commaHeaders.insert("if-none-match");
	commaHeaders.insert("pragma");
	commaHeaders.insert("te");
	commaHeaders.insert("trailer");
	commaHeaders.insert("transfer-encoding");
	commaHeaders.insert("upgrade");
	commaHeaders.insert("vary");
	commaHeaders.insert("via");
	commaHeaders.insert("warning");
}

void Request::_parseHeader(const std::string &key, const std::string &value)
{
	std::string forbiddenChars = " \"(),/:;<=>?@[]{}\\";

	if (key.empty() || value.empty())
		throw RequestException("400 Bad Request");

	// ? key grammar check
	if (key.find_first_of(forbiddenChars) != std::string::npos)
		throw RequestException("400 Bad Request");
}

void Request::_insertHeader(std::string &key, const std::string &value, const std::set<std::string> commaHeaders)
{
	std::transform(key.begin(), key.end(), key.begin(),
				   ::toLowerCase); // convert the entire string to lowercase. as the key is case-insensitive

	if (key == "cookie" && _headers.find(key) != _headers.end())
		_headers.find(key)->second += "; " + value;
	else if (_headers.find(key) != _headers.end()) // concat with comma, otherwise ignore new ones
	{
		if (commaHeaders.find(key) != commaHeaders.end())
			_headers.find(key)->second += ", " + value;
		else
			return;
	}
	else // store normal headers
		_headers.insert(std::pair<std::string, std::string>(key, value));
}

void Request::_parseAllHeaders(const std::vector<std::string> &lines)
{
	std::set<std::string> commaHeaders;
	std::string key, value;

	initCommaHeaders(commaHeaders);
	for (size_t i = 1; i < lines.size(); i++)
	{
		if (lines[i].empty())
			throw RequestException("400 Bad Request");

		key = lines[i].substr(0, lines[i].find_first_of(":"));
		value = trim(lines[i].substr(lines[i].find_first_of(":") + 1));

		_parseHeader(key, value);
		_insertHeader(key, value, commaHeaders);
	}
}

void Request::_parseFirstLine(const std::vector<std::string> &lines)
{
	std::vector<std::string> fields;
	std::string method, path, version, first_line;

	first_line = lines.front();
	split(first_line, fields, " ");
	if (fields.size() != 3)
		throw RequestException("400 Bad Request");

	method = fields[0];
	path = fields[1]; // ? i could check for the length of the uri, if its too long, throw 414 URI Too Long
	version = fields[2];
	if (!path.empty() && (method == "GET" || method == "POST" || method == "DELETE") && version == "HTTP/1.1") // todo : i will add the rest of the methods later
	{
		_path = path;
		_method = method;
		_version = version;
		if (!isMethodAllowed(method))
			throw RequestException("405 Method Not Allowed");
	}
	else
		throw RequestException("400 Bad Request");
}
static std::string getCookieValue(const std::string &cookieHeader, const std::string &name)
{
	std::string search = name + "=";
	size_t start = cookieHeader.find(search);

	if (start == std::string::npos)
		return "";

	start += search.length(); // skips "session_id="
	size_t end = cookieHeader.find(';', start);

	if (end == std::string::npos)
		return cookieHeader.substr(start); // last cookie, no ";" after it
	return cookieHeader.substr(start, end - start);
}

void Request::parseCookies()
{
	sessionManager &manager = sessionManager::getInstance();
	std::string cookieHeader, sessionId;

	if (getHeaderValue("cookie") == "")
	{
		DEBUG_INFO2("No cookies found, so creating a new one");
		this->_session = manager.createSession();
		return;
	}

	cookieHeader = getHeaderValue("cookie");
	sessionId = getCookieValue(cookieHeader, "session_id");
	this->_session = manager.getSession(sessionId);

	if (this->_session == NULL) // not a valid session id
	{
		DEBUG_INFO2("Found Cookie, but not valid");
		this->_session = manager.createSession(); // generate a new session
		return;
	}

	DEBUG_INFO2("Found Cookie, and its valid");
	DEBUG_INFO2("Now filling the cookie data");
	std::istringstream ss(cookieHeader);
	std::string token;

	while (std::getline(ss, token, ';'))
	{
		size_t eq = token.find("=");
		if (eq == std::string::npos)
			continue;

		std::string key = trim(token.substr(0, eq));
		std::string value = trim(token.substr(eq + 1));

		if (key != "session_id" && key != "Expires" && key != "Path" && key != "HttpOnly")
			this->_session->setData(key, value);
	}
	this->_is_new_session = false;
}

void Request::requestParser(const std::string &raw)
{
	std::vector<std::string> lines;
	std::string del = "\r\n";

	split(raw, lines, del);

	_parseFirstLine(lines);
	_parseAllHeaders(lines);
	// NOTE: importent to add request methods that have body here like "put" if you use it
	if (!to_integer<std::string, size_t>(getHeaderValue("Content-Length"), _content_size) && _method == "POST")
	{
		DEBUG_ERROR("request parser: invalid Content-Length");
		throw RequestException("400 Bad Request");
	}
	else if (_content_size > 0 && _method != "POST")
	{
		DEBUG_ERROR("request parser: there is no meaning of sending a body with GET request");
		throw RequestException("400 Bad Request");
	}
	if (getHeaderValue("transfer-encoding") != "" && getHeaderValue("transfer-encoding") != "identity")
		throw RequestException("400 Bad Request");
	_match_loc = getMatchedLocation();
	if (!_match_loc)
		throw RequestException("400 Bad Request");
	parseCookies();
	DEBUG_INFO2("#######################Database Information#######################");
	sessionManager &manager = sessionManager::getInstance();
	std::map<std::string, Session> db = manager.getDatabase();
	std::cout << "Database size: " << db.size() << std::endl;
	for (std::map<std::string, Session>::iterator it = db.begin(); it != db.end(); it++)
	{
		std::cout << "Session id: " << it->first << std::endl;
		// std::cout << it->second.getData()[0] << std::endl;
		std::cout << "\n";
	}
	DEBUG_INFO2("##################################################################");
}

// change name to checkCGI request
bool Request::isCGI()
{
	const std::string uri = getPath();
	const std::string ext = getFileExtension(uri);
	bool hasExtAtEnd = !ext.empty() && _match_loc->hasCgiForExt(ext);
	// is not cgi at all because the match location doesn't have cgiExt and cgiPath (use matchLog->hasCGI for that)
	// return false
	if (!_match_loc->hasCgi())
		return false;
	_is_cgi = true;
	// its cgi and its post method and its have no .[ext] at end of path so its for upload cgi script and this is response part
	// set is_cgi true and return false
	if (_method == "POST" && !hasExtAtEnd)
		return false;
	// the only all methods remain which is get/post/delete for run or delete cgi is require scirpt name with ext
	else if (!hasExtAtEnd)
		throw RequestException("400 Bad Request");
	// its cgi and its delete method and its have .[ext] at end of path so its for delete cgi and this is response part
	// set is_cgi true and return false
	else if (_method == "DELETE")
		return false;
	// its cgi and it get or post and its has the .[ext] at end so request for run cgi
	// set is_cgi true and return true and here my cgi work should be run
	else if (!(_method == "POST" || _method == "GET"))
		return false;
	std::string uploadStore = (!_match_loc->getUploadStore().empty())
								  ? _match_loc->getUploadStore()
								  : RespDefaults::CGI_STORE;
	// NOTE: also i should store info like the path and everything so i don't need to use look for it next time
	_file_path = uploadStore + getFileName(uri);
	_cgi_path = _match_loc->getCgiPathForExt(ext); // storing the interpreter path

	if (access(_file_path.c_str(), X_OK) == -1)
		throw RequestException("400 Bad Request");
	DEBUG_INFO2("This requist is a CGI");
	return (true);
}

const LocationConfig *Request::getMatchedLocation() const
{
	const std::string &uri = getPath();
	const std::vector<LocationConfig> &locs = _serverConf.getLocations();

	const LocationConfig *best = NULL;
	size_t best_len = 0;

	for (size_t i = 0; i < locs.size(); ++i)
	{
		const std::string &loc_path = locs[i].getPath();

		if (uri.compare(0, loc_path.size(), loc_path) == 0)
		{
			if (loc_path.size() == uri.size() ||
				uri[loc_path.size()] == '/' ||
				loc_path[loc_path.size() - 1] == '/')
			{
				if (loc_path.size() > best_len)
				{
					best = &locs[i];
					best_len = loc_path.size();
				}
			}
		}
	}
	return best;
}
