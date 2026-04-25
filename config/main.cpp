#include "Lexer.hpp"
#include "Parser.hpp"

using namespace std;

int main()
{
	string fileName = "default2.conf";
	ifstream readfile(fileName.c_str());
	string line, all_lines;
	std::vector<Token> allTokens;

	if (!readfile.is_open())
	{
		cerr << "Error: Cannot open file " + fileName << endl;
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
	Config configObj(allTokens);

	return 0;
}

//