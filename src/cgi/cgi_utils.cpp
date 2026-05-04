
#include "../../includes/container.hpp"
/*
char **CGIClient::buildEnv()
{
    std::vector<std::string> envVars = {
        "REQUEST_METHOD=GET",
        "QUERY_STRING=name=Ali&age=22",
        "CONTENT_TYPE=application/x-www-form-urlencoded",
        "CONTENT_LENGTH=0",
        "SCRIPT_NAME=/cgi-bin/hello.py",
        "PATH_INFO=/cgi-bin/hello.py",
        "SERVER_NAME=localhost",
        "SERVER_PORT=8080",
        "SERVER_PROTOCOL=HTTP/1.0",
        "HTTP_HOST=localhost:8080",
        "HTTP_USER_AGENT=Mozilla/5.0",
        "GATEWAY_INTERFACE=CGI/1.0",
    };

    char **env = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); i++)
    {
        env[i] = new char[envVars[i].size() + 1];
        std::strcpy(env[i], envVars[i].c_str());
    }
    env[envVars.size()] = NULL; 
    return env;
}
*/
