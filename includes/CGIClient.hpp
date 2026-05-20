#ifndef CGICLIENT_HPP
#define CGICLIENT_HPP

#include <sys/epoll.h>
#include "Client.hpp"
#include "EpollHold.hpp"

class CGIClient: public Client
{
  private:
    int _pid; // for child
    bool _socket_done;
    bool _cgi_pipe_done;
    int _pipe_in[2];
    int _pipe_out[2];
    t_epollhold _pipe_in_hold;
    t_epollhold _pipe_out_hold;
  public:
    CGIClient(int fd, int epfd);
    CGIClient(Client &cl);
    ~CGIClient();
    void handel(int fd, uint32_t evs);
    // methods
    void setupPipes();
    char **buildEnv();
    void ft_exec();

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
