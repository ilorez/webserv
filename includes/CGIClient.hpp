#ifndef CGICLIENT_HPP
#define CGICLIENT_HPP

#include "Client.hpp"
#include <ctime>
#include <sys/epoll.h>

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
    size_t _read_counter;
    bool _socket_done;
    bool _cgi_pipe_done;
    //bool _download_switch; // on: for socket, off: for pipe
    //bool _upload_switch;  // off: for pipe, on: for socket 
  public:
    CGIClient(int fd);
    CGIClient(Client &cl);
    ~CGIClient();
    void disconnect(int epfd);
    void handel(int fd, uint32_t evs);
    // methods
    void setupPipes();
    //char **buildEnv();
    void ft_exec(int epfd);
    //
    // writeBodyChunk() // write POST body chunk to _pipe_in[1]
    // readOutputChunk() // read CGI response chunk from _pipe_out[0] into writeBuffer
    // isBodyFullySent()// check POST body fully written
    // parseCGIHeaders() // strip CGI headers from output (confirm with Ali)
    
    // i/o
    void writeToReadBuffer();
    void writeToPipe();
    void writeToWriteBuffer();
    void writeToSocket();

    void registerPipeOut(int epfd); // : register pipe_out[0] with EPOLLIN
    void registerPipeIn(int epfd); // : register pipe_in[1] with EPOLLOUT (POST only)
    
    // TODO:
    // unregisterPipes(): remove both pipe fds from epoll
    // closePipes()     : close all open pipe fds (check != -1 before closing)
    // killChild() → kill(child_pid, SIGKILL) + waitpid

  private:
    CGIClient(const CGIClient &);
    CGIClient& operator=(const CGIClient &);
};

#endif
