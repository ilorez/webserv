#include "./includes/container.hpp"
#include "includes/Debug.hpp"

void signalHandeler(int sig)
{
  (void) sig;
  DEBUG_INFO("Got exit Signal");
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << ERROR_MSG << "usage: ./webserv <config_file>" << std::endl;
    return 1;
  }
  signal(SIGINT, signalHandeler);
  signal(SIGTERM, signalHandeler);

  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << ERROR_MSG << "cannot open file: " << argv[1] << std::endl;
    return 1;
  }

  std::string content = ft_readFile(argv[1]);
  file.close();
  try
  {
    Scanner tokenizer(content);
    if (tokenizer.getHadError())
      return 1;

    DEBUG_INFO("tokens complete");
    Config config(tokenizer.getTokens());
    DEBUG_INFO("config complete");
    std::vector<ServerConfig>& servers = config.getServers();
    DEBUG_INFO("getServers complete");
    Server nginx(servers);
    DEBUG_INFO("add servers to nginx server complete");

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
    return e.status();
  }
  catch (const std::exception &e)
  {
    std::cerr << ERROR_MSG << "error: " << e.what() << std::endl;
  }
  // TODO: free_all
}
