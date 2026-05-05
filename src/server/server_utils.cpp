
#include "../../includes/container.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>

// add client
void Server::_addClient(int client_fd) {
  // applying non-blocking mode to everyclient fd
  fcntl(client_fd, F_SETFL, O_NONBLOCK);

  Client *cl = _clients.addClient(client_fd);
  t_epollhold &eh = cl->getClSockHolder();
  // adding to epoll queu
  _switchEpollRegisration( &eh, EPOLLIN | EPOLLHUP);
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// add client
void Server::_addSocketToEpoll(int sfd) {
  _srvsock_hold.fd = sfd;
  // applying non-blocking
  fcntl(sfd, F_SETFL, O_NONBLOCK);
  // adding to epoll queu
  _switchEpollRegisration( &_srvsock_hold, EPOLLIN);
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sfd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// switch epoll regitration
void Server::_switchEpollRegisration(t_epollhold *eh, uint32_t ev)
{
  _epoll_event.data.ptr = eh;
  _epoll_event.events = ev;
}

void Server::newconnection(socklen_t size_socket)
{
  int client_fd = accept(_socket_fd, (struct sockaddr *)(&_addr), (socklen_t *)&size_socket);
  DEBUG_INFO("------------New Request-----------");
  if (client_fd < 0)
    throw ServerException("accept() failed.");
  // adding to clinet list
  this->_addClient(client_fd);
  DEBUG_INFO("New Client Added");
}

void Server::readheaders(Client *cl)
{
  std::string headers = cl->readHeaders();
  if (headers.empty())
    return;
  try {
    DEBUG_INFO("Request");
    cl->getReq().requestParser(headers);
    if (cl->getReq().isCGI())
    {
      // in case of CGI i'm upgrading the Client class to CGI by using copy constructor
      CGIClient* cgi = _clients.updateToCGI(cl->getFd());
      cgi->preSetup();
      return;
    }
  } catch (const std::exception &e){
    std::cerr << ERROR_MSG << "parsing headers: "<< e.what() << std::endl;
    // send bad request
    cl->callError(400);
  }
  cl->preSetup();
}
