
#include "../../includes/container.hpp"
/*
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
// */
char **CGIClient::buildEnv()
{
    std::vector<std::string> envVars;

    // Required CGI vars from request
    envVars.push_back("REQUEST_METHOD=" + _req.getMethod());
    envVars.push_back("SCRIPT_NAME=" + _req.getFilePath());
    envVars.push_back("PATH_INFO=" + _req.getFilePath());
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.0");
    envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // Map HTTP headers → CGI vars
    std::map<std::string, std::string>& headers = _req.getHeaders();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::string key = it->first;

        // Special cases: no HTTP_ prefix
        if (key == "Content-Type" || key == "content-type")
            envVars.push_back("CONTENT_TYPE=" + it->second);
        else if (key == "Content-Length" || key == "content-length")
            envVars.push_back("CONTENT_LENGTH=" + it->second);
        else
        {
            // Uppercase and prefix with HTTP_
            std::string cgiKey = "HTTP_";
            for (size_t i = 0; i < key.size(); ++i)
                cgiKey += (key[i] == '-') ? '_' : std::toupper(key[i]);
            envVars.push_back(cgiKey + "=" + it->second);
        }
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
  // register socket again with EPOLLIN
  struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
  epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
  // i should update the 
  // if i already ready body or the request is get not post
  // i should never register the socket EPOLLIN in that case because its will never fired
  this->setupPipes();
  if (_req.getMethod() == "POST")
  {
    _read_counter = _readBuffer.size();
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
