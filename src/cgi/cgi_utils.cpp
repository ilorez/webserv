
#include "../../includes/container.hpp"

char **CGIClient::buildEnv()
{
    std::vector<std::string> envVars;
    std::map<std::string, std::string>& map = _req.getHeaders();
    for (std::map <std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
    {
      envVars.push_back(it->first + "=" +  it->second);
      std::cout << "env back: " << envVars.back() << std::endl;
    }
    char **env = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); i++)
    {
        env[i] = new char[envVars[i].size() + 1];
        std::strcpy(env[i], envVars[i].c_str());
    }
    env[envVars.size()] = NULL; 
    return env;
}

void CGIClient::preSetup()
{
  // if i already ready body or the request is get not post
  // i should never register the socket EPOLLIN in that case because its will never fired
  if (_req.getMethod() == "POST")
  {
    if (_readBuffer.size() >= _req.getContentLen())
        this->turnToPipe();
  }
  else
  {
    this->removeEpollinEventFromSocket();
    _socket_done = true;
  }
  // run setup cgi
  this->ft_exec();
}
