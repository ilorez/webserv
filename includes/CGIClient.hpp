#ifndef CGICLIENT_HPP
#define CGICLIENT_HPP

#include <sys/epoll.h>
#include "Client.hpp"
#include "EpollHold.hpp"

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
    bool _cgi_headers_parsed;
    size_t _read_counter;
    bool _socket_done;
    bool _cgi_pipe_done;
    t_epollhold _pipe_in_hold;
    t_epollhold _pipe_out_hold;
    int _pid; // for child
    //bool _download_switch; // on: for socket, off: for pipe
    //bool _upload_switch;  // off: for pipe, on: for socket 
  public:
    CGIClient(int fd, int epfd);
    CGIClient(Client &cl);
    ~CGIClient();
    void disconnect(int epfd);
    void handel(int fd, uint32_t evs);
    // methods
    void setupPipes();
    //char **buildEnv();
    void ft_exec();

    // parseCGIHeaders() // check CGI headers (Ali)
    virtual void preSetup();
    
    // i/o
    void writeToReadBuffer();
    void writeToPipe();
    void writeToWriteBuffer();
    void writeToSocket();

    void removeEpollinEventFromSocket();
    void turnToPipe();
    void registerPipeOut(); // : register pipe_out[0] with EPOLLIN
    void registerPipeIn(); // : register pipe_in[1] with EPOLLOUT (POST only)
    
  private:
    CGIClient(const CGIClient &);
    CGIClient& operator=(const CGIClient &);
};

#endif
