void Request::_readBody(const std::string &raw, const std::string &sep)
{
	std::string bodySec;
	int size;

	if (_headers.find("transfer-encoding") == _headers.end() && _headers.find("content-length") == _headers.end()) // there is no body
		return;

	bodySec = raw.substr(raw.find(sep) + 4);

	if (_headers.find("transfer-encoding") != _headers.end())
	{
		// read chunk by chunk // transfer-encoding: gzip, chunked
	}
	else if (_headers.find("content-length") != _headers.end())
	{
		if (_headers.find("content-length")->second.find_first_not_of("0123456789") != std::string::npos) // check if the content-length value is invalid (negative, duplicated ...)
			throw RequestException("400 Bad Request");

		size = atoi((_headers.find("content-length")->second).c_str());
		if (size == -1 || size > BODY_SIZE_LIMIT)
			throw RequestException("413 Content Too Large");

		_body += bodySec.substr(0, size);
	}
}
