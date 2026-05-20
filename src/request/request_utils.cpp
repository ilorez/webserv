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

	if (key == "set-cookie") // store it in a seperated vector, as its an exception
		_setCookieHeaders.push_back(std::pair<std::string, std::string>(key, value));
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

void Request::requestParser(const std::string &raw)
{
	std::vector<std::string> lines;
	std::string del = "\r\n";

	split(raw, lines, del);

	_parseFirstLine(lines);
	_parseAllHeaders(lines);
	// NOTE: importent to add request methods that have body here like "put" if you use it
	if (_method == "POST" && !to_integer<std::string, size_t>(getHeaderValue("Content-Length"), _content_size))
	{
		DEBUG_ERROR("request parser: invalid Content-Length");
		throw RequestException("400 Bad Request");
	}
  _match_loc = getMatchedLocation();
	if (!_match_loc)
    throw RequestException("400 Bad Request");
}

// change name to checkCGI request 
bool Request::isCGI()
{
  const std::string uri = getPath();
  bool hasExtAtEnd = endsWith(uri, _match_loc->getCgiExt());
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
                                : Default::CGI_STORE;
  // NOTE: also i should store info like the path and everything so i don't need to use look for it next time
	_file_path =  uploadStore + getFileName(uri);
  if (access(_file_path.c_str(), X_OK) == -1)
    throw RequestException("400 Bad Request");
  DEBUG_INFO("This requist is a CGI");
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
