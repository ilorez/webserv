
#include "../../includes/container.hpp"
#include <sys/epoll.h>

// #constructors
Server::Server(): _socket_fd(-1), _port(8080), _epoll_fd(-1), _ip("127.0.0.1")
{
  _epoll_event.events = EPOLLIN;
  _srvsock_hold.is_cgi = false;
  _srvsock_hold.fd = -1;
  _srvsock_hold.cl = NULL;
  _epoll_event.data.ptr = NULL;
}

Server::~Server() 
{ 
  if (_socket_fd > -1)
    close(_socket_fd);
}

Server::Server(const Server &o) {
  (void) o;
}

Server& Server::operator=(const Server &o) {
  (void) o;
  return *this;
}

void Server::run() 
{
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

void Server::_handelClient(socklen_t size_socket)
{
  DEBUG_INFO("------------------");
  int n = epoll_wait(_epoll_fd, _events, MAX_EVENTS, EPOLL_WAIT_TIMEOUT);
  if (n == -1)
    throw ServerException("epoll_wait: failed");
  for (int i = 0; i < n; i++)
  {
    t_epollhold *eh = static_cast<t_epollhold*>(_events[i].data.ptr);
    if (!eh)
    {
        DEBUG_ERROR("epoll data ptr is invalid");
        continue;
    }
    DEBUG_INFO("event fired on fd: " + to_string98(eh->fd));
    if (eh->fd == _socket_fd)
      this->newconnection(size_socket);
    else if (_events[i].events & EPOLLIN || _events[i].events & EPOLLOUT)
    {
      if (eh->cl->isTimedOut(TIMEOUT_SECONDS))
        continue;
      else if (eh->is_cgi)
        eh->cgi->handel(eh->fd, _events[i].events);
      else if (eh->cl->getState() == READING_HEADERS)
        this->readheaders(eh->cl);
      else
        eh->cl->handel(0, _events[i].events);
      if (eh->cl->getState() ==  DONE)
        eh->cl->forceTimeout();
      else
        eh->cl->updateLastActivity();
    }
    else if (_events[i].events & ( EPOLLHUP | EPOLLERR | EPOLLRDHUP))
      eh->cl->forceTimeout();
    else 
      DEBUG_ERROR("Unknown event firedon");
  }
  _clients.checkTimeout();
}

