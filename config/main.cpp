#include "Lexer.hpp"
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "locationConfig.hpp"

using namespace std;

void printLocationData(const LocationConfig &loc)
{
	cout << "\t  path:              " << loc.getPath() << "\n";
	cout << "\t  root:              " << loc.getRoot() << "\n";
	cout << "\t  autoindex:         " << (loc.getAutoindex() ? "on" : "off") << "\n";
	cout << "\t  clientMaxBodySize: " << loc.getClientMaxBodySize() << "\n";

	const vector<string> &methods = loc.getMethods();
	cout << "\t  methods:           ";
	if (methods.empty())
		cout << "(inherited)";
	for (size_t i = 0; i < methods.size(); i++)
		cout << methods[i] << (i + 1 < methods.size() ? " " : "");
	cout << "\n";

	const vector<string> &index = loc.getIndex();
	cout << "\t  index:             ";
	for (size_t i = 0; i < index.size(); i++)
		cout << index[i] << (i + 1 < index.size() ? " " : "");
	cout << "\n";

	if (loc.getReturnCode())
	{
		cout << "\t  return code:       " << loc.getReturnCode() << "\n";
		if (!loc.getReturnUrl().empty())
			cout << "\t  return url:        " << loc.getReturnUrl() << "\n";
	}

	if (loc.hasCgi())
	{
		cout << "\t  cgi_ext:           " << loc.getCgiExt() << "\n";
		cout << "\t  cgi_path:          " << loc.getCgiPath() << "\n";
	}

	if (!loc.getUploadStore().empty())
		cout << "\t  upload_store:      " << loc.getUploadStore() << "\n";

	cout << "\t  " << string(50, '-') << "\n";
}
void printServerData(const vector<ServerConfig> &servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		const ServerConfig &s = servers[i];

		cout << string(60, '=') << "\n";
		cout << "Server #" << i + 1 << "\n";
		cout << string(60, '=') << "\n";
		cout << "  host:              " << s.getHost() << "\n";
		cout << "  port:              " << s.getPort() << "\n";
		cout << "  server_name:       " << s.getServerName() << "\n";
		cout << "  root:              " << s.getRoot() << "\n";
		cout << "  clientMaxBodySize: " << s.getClientMaxBodySize() << "\n";
		cout << "  autoindex:         " << (s.getAutoIndex() ? "on" : "off") << "\n";

		const vector<string> &index = s.getIndex();
		cout << "  index:             ";
		for (size_t j = 0; j < index.size(); j++)
			cout << index[j] << (j + 1 < index.size() ? " " : "");
		cout << "\n";

		const map<int, string> &errorPages = s.getErrorPages();
		if (!errorPages.empty())
		{
			cout << "  error_pages:\n";
			for (map<int, string>::const_iterator it = errorPages.begin(); it != errorPages.end(); it++)
				cout << "    " << it->first << " -> " << it->second << "\n";
		}

		const vector<LocationConfig> &locations = s.getLocations();
		if (locations.empty())
			cout << "  locations:         (none)\n";
		else
		{
			cout << "  locations: (" << locations.size() << ")\n";
			for (size_t j = 0; j < locations.size(); j++)
			{
				cout << "\t[" << j + 1 << "] ";
				cout << locations[j].getPath() << "\n";
				cout << "\t  " << string(50, '-') << "\n";
				printLocationData(locations[j]);
			}
		}
		cout << "\n";
	}
}

int main()
{
	std::string fileName = "default2.conf";
	std::ifstream readfile(fileName.c_str());
	std::string line, all_lines;
	std::vector<Token> allTokens;

	if (!readfile.is_open())
	{
		std::cerr << "Error: Cannot open file " + fileName << std::endl;
		return (1);
	}

	while (getline(readfile, line))
		all_lines += (line + "\n");
	readfile.close();

	// Start lexing
	Scanner tokenizer(all_lines);

	if (tokenizer.getHadError())
		return 1;

	allTokens = tokenizer.getTokens();

	// start parsing
	try
	{
		Config configObj(allTokens);
		printServerData(configObj.getServers());
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}

//