
#include "../../includes/container.hpp"
#include <sys/epoll.h>

Server::Server()
{
  _port = 8080;
  _ip = "127.0.0.1";
  _epoll_event.events = EPOLLIN;
  _status_error = 0;
}

Server::~Server() { close(_socket_fd);
}
void Server::run() {
  this->_initSocket(); 
  socklen_t size_socket = sizeof(_addr);
  _epoll_fd = epoll_create(1);
  if (_epoll_fd == -1)
    throw ServerException("server run: epoll_create fail");
  _clients.setEpfd(_epoll_fd);
  this->_addSocketToEpoll(_socket_fd);
  while (1)
    this->_handelClient(size_socket);
}

void Server::_initSocket()
{
  // AF_INET: address family ipv4
  // SOCK_STREAM: socket type: stream-based (as opposed to SOCK_DGRAM for UDP)
  // IPPROTO_TCP protocol: TCP 
  _socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_socket_fd < 0)
    throw ServerException("socket() failed.");
  // apply non-bloacking mode to listening socket fd
  int opt = 1;
  // SOL_SOCKET: the "level" you're setting an option at the socket layer
  // SO_REUSEADDR: for reusing a local address that still in TIME_WAIT
  // so this method used to change on a setting of a socket and its require for that the
  // socket and the level of option you want to change on that socket and the option name 
  // or value after that the value and size you want the setsockopt to take from 
  // that value
  setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;// ipv4 family
  _addr.sin_port = htons(_port);
  // The htons() function converts the unsigned short integer hostshort from
  // host byte order to network byte order.
  _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
  // convert from string to 32bit format
  if (bind(_socket_fd, (struct sockaddr *)(&_addr), sizeof(_addr)))
  {
    // bind is like setsockopt its just tell the os that the socket belongs
    // to this address and this port
    throw ServerException("bind() failed with port" + to_string98(_port));
  }
  // listen()  marks  the  socket referred to by sockfd as a passive socket,
  // that is, as a socket that will be used to  accept  incoming  connection
  // requests using accept(2).
  if (listen(_socket_fd, MAX_CONX_QUEUE))
    throw ServerException("listen() failed.");

  std::cout << INFO_MSG << "listen on http://" << inet_ntoa(_addr.sin_addr) << ":" << to_string98(ntohs(_addr.sin_port)) << std::endl;
}

void Server::_handelClient(socklen_t size_socket)
{
  int n = epoll_wait(_epoll_fd, _events, MAX_EVENTS, EPOLL_WAIT_TIMEOUT);
  if (n == -1)
  {
    DEBUG_ERROR("handelClient: epoll wait fail");
    return ;
  }
  for (int i = 0; i < n; i++)
  {
    // new client
    t_epollhold *eh = static_cast<t_epollhold*>(_events[i].data.ptr);
    if (!eh){
        DEBUG_ERROR("epoll data ptr is invalid");
        /*erroo*/ continue;}
    if (eh->fd == _socket_fd)
      this->newconnection(size_socket);
    // EPOLLIN fires on client_fd:
    else if (_events[i].events & EPOLLIN || _events[i].events & EPOLLOUT)
    {
      if (eh->is_cgi)
        eh->cgi->handel(eh->fd, _events[i].events);
      else if (_events[i].events & EPOLLIN)
      {
        this->readrequest(eh->cl);
        if (eh->cl->getState() == PROCESSING || eh->cl->getState() == SENDING ){
          _switchEpollRegisration(eh, EPOLLOUT);
          epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, eh->cl->getFd(), &_epoll_event);}
      }
      else if (_events[i].events & EPOLLOUT)
        this->sendresponse(eh->cl);
      // update clinet last activity to now
      if (eh->cl->getState() ==  DONE)// should work for cgi and cl
        _clients.disconnect(eh->cl->getFd());
        // TODO: delete eh
      else
        eh->cl->updateLastActivity();
    }
    else if (_events[i].events & EPOLLHUP || _events[i].events & EPOLLERR)
    {
      // client disconnected or error
      _clients.disconnect(eh->cl->getFd());
      // TODO: delete eh
    }
  }
  // INFO: checking timeout everytime can reduce performance
  // check timeout n=0
  // TODO: its not good to update it everytime, the only thing i need from socket is fd, the good thing is that socket created one time
  // and things i need to change for remove remove it from client is more then just few bytes
  // what you need is just the give the socket fd the max future time so its never timed out, and you never update it
  _clients.getClient(_socket_fd)->updateLastActivity();
  _clients.checkTimeout();
}

