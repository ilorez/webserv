
#include "../../includes/container.hpp"

void CGIClient::freeEnv(char **env)
{
    if (!env)
        return;
    for (int i = 0; env[i] != NULL; i++)
        delete[] env[i];
    delete[] env;
}

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
    std::map<std::string, std::string> &headers = _req.getHeaders();
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

    std::map<std::string, std::string> &cookies = _req.getSession()->getData();
    for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
    {
        std::string key = "COOKIE_";
        for (size_t i = 0; i < it->first.size(); i++)
            key += (it->first[i] == '-') ? '_' : std::toupper(it->first[i]);
        envVars.push_back(key + "=" + it->second);
    }
    envVars.push_back("COOKIE_SESSION_ID=" + _req.getSession()->getId());

    char **env = new char *[envVars.size() + 1];
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
  epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
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
  this->ftExec();
}

bool CGIClient::doneCheckCgiOutputHeaders(std::string &chunk)
{
      if (_got_headers_end)
        return true;
      size_t pos = chunk.find("\n\n");
      if (pos == std::string::npos)
      {
          pos = chunk.find("\r\n\r\n");
          if (pos == std::string::npos)
            return false;
          replace_all(_writeBuffer, "\r\n", "\n");
      }
      _got_headers_end = true;
      std::string status = "200 OK";
      size_t status_pos = _writeBuffer.find("Status:");
      if (status_pos != std::string::npos && status_pos < pos)
      {
          size_t end = _writeBuffer.find("\n", status_pos);
          status = _writeBuffer.substr(status_pos + 7, end - (status_pos + 7));
  
          size_t start = status.find_first_not_of(" \t");
          if (start != std::string::npos)
              status = status.substr(start);
      }
  
      size_t loc_pos = _writeBuffer.find("Location:");
      if (loc_pos != std::string::npos && loc_pos < pos)
      {
          size_t loc_end = _writeBuffer.find("\n", loc_pos);
          std::string loc_val = _writeBuffer.substr(loc_pos + 9,
                                                    loc_end - (loc_pos + 9));
  
          size_t start = loc_val.find_first_not_of(" \t");
          if (start != std::string::npos)
              loc_val = loc_val.substr(start);
  
          if (!loc_val.empty() && status == "200 OK")
              status = "302 Found";
      }
  
      _writeBuffer =
          "HTTP/1.0 " + status + "\r\n" +
          _writeBuffer;
      return true;
}
