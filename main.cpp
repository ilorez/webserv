#include "./includes/container.hpp"

using namespace std;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << ERROR_MSG << "usage: ./webserv <config_file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << ERROR_MSG << "cannot open file: " << argv[1] << std::endl;
    return 1;
  }

  std::string content = ft_readFile(argv[1]);

  try
  {
    Scanner tokenizer(content);
    if (tokenizer.getHadError())
      return 1;

    Config config(tokenizer.getTokens());
    Server nginx(config.getServers());

    nginx.run();
  }
  catch (const ServerException &e)
  {
    std::cerr << ERROR_MSG << "server: " << e.what() << std::endl;
  }
  catch (const RequestException &e)
  {
    std::cerr << ERROR_MSG << "request: " << e.what() << std::endl;
  }
  catch (const ResponseException &e)
  {
    std::cerr << ERROR_MSG << "response: " << e.what() << std::endl;
  }
  catch (const CGIException &e)
  {
    std::cerr << ERROR_MSG << "cgi: " << e.what() << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << ERROR_MSG << "error: " << e.what() << std::endl;
  }
}
