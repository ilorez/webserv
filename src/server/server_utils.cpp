
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

void Server::readheaders(Client *cl)
{
  // read from user client socket
  // read to the line before \r\n\r\n 
  char tmp[BUF_SIZE];
  size_t pos = 0;
  size_t tmppos;
  while (1)
  {
    int bytes = recv(cl->getFd(), tmp, BUF_SIZE, 0);
    if (bytes <= 0)
      break;
    cl->appendToReadBuffer(tmp, bytes);
    tmppos = cl->getReadBuffer().find("\r\n\r\n");
    if ( tmppos != std::string::npos)
    {
      pos = tmppos; 
      break;
    }
    if (cl->getReadBuffer().size() > MAX_HEADER_SIZE)
    {
      //TODO: send error 431 to client 
      // 431 Request Header Fields Too Large
      _clients.disconnect(cl->getFd());
    }
  }
  if (pos < 1)
  {
    DEBUG_WARN("Empty request");
    // TODO: send bad request
    _clients.disconnect(cl->getFd());
    return;
  }
  std::string headers = cl->getReadBuffer().substr(0, pos);
  //std::cout << headers << std::endl;
  cl->setReadBuffer(cl->getReadBuffer().substr(pos+4));
  DEBUG_INFO("Request");
  req.requestParser(headers);
  if (req.getMethod() == "POST")
    cl->setState(READING_BODY);
  else
    cl->setState(PROCESSING);
}
