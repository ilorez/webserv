#ifndef CGICLIENT_HPP
#define CGICLIENT_HPP

#include "Client.hpp"
#include <ctime>

class CGIClient: public Client
{
// CLASS DESIGN
// ------------
// CGIClient : public Client
//   - int pipe_in[2]      (parent writes → CGI stdin)
//   - int pipe_out[2]     (parent reads  ← CGI stdout)
//   - pid_t child_pid
//   - bool cgi_headers_parsed
//   - virtual disconnect() → kill child, close + unreg pipe fds, then base cleanup
  private:
    int _pipe_in[2];
    int _pipe_out[2];
    pid_t _child_pid;
    bool _cgi_headers_parsed;
  public:
    CGIClient(int fd);
    CGIClient(Client &cl);
    ~CGIClient();
    void disconnect(int epfd);
  private:
    CGIClient(const CGIClient &);
    CGIClient& operator=(const CGIClient &);
};

#endif
