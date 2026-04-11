
#include "../../includes/container.hpp"

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

