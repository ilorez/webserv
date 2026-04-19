
#include "../../includes/container.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>

// add client
void Server::_addClient(int client_fd) {
  // applying non-blocking mode to everyclient fd
  fcntl(client_fd, F_SETFL, O_NONBLOCK);
  
  // adding to epoll queu
  _switchEpollRegisration(client_fd, EPOLLIN);
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// switch epoll regitration
void Server::_switchEpollRegisration(int client_fd, uint32_t ev)
{
  _epoll_event.data.fd = client_fd;
  _epoll_event.events = ev;
}



void Server::newconnection(socklen_t size_socket)
{
  int client_fd = accept(_socket_fd, (struct sockaddr *)(&_addr), (socklen_t *)&size_socket);
  DEBUG_INFO("------------New Request-----------");
  if (client_fd < 0)
    throw ServerException("accept() failed.");
  // adding to clinet list
  _clients.addClient(client_fd);
  this->_addClient(client_fd);
}


bool Server::createTmpFile(Client *cl)
{
  cl->getReq().setTmpFileName(makeTmpPath(cl->getFd()));
  int tfd = open(cl->getReq().getTmpFileName().c_str(), O_RDWR | O_APPEND | O_CREAT);
  if (tfd < 0)
  {
    DEBUG_ERROR("readFromSocket: could not create tmp file");
    return false;
  }
  cl->getReq().setTmpFd(tfd);
  write(cl->getReq().getTmpFd(), cl->getReadBuffer().c_str(), cl->getReadBuffer().size());
  cl->getReq().setBytesCounter(cl->getReadBuffer().size());
  cl->clearReadBuffer();
  return true;
}


void Server::callError(int err_code, Client *cl)
{
    _status_error = err_code;
    cl->setState(PROCESSING);
}

