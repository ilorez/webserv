
#include "../../includes/container.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <sys/epoll.h>

// add client
void Server::_addClient(int client_fd, ServerConfig &sc) {
  // applying non-blocking mode to everyclient fd
  fcntl(client_fd, F_SETFL, O_NONBLOCK);

  Client *cl = _clients.addClient(client_fd);
  cl->getReq().setServerConfig(sc);
  t_epollhold &eh = cl->getClSockHolder();
  _switchEpollRegisration( &eh, EPOLLIN | EPOLLRDHUP);
  // adding to epoll queu
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// add client
void Server::_addSocketToEpoll(t_epollhold &_srv_hold, int sfd) {
  _srv_hold.fd = sfd;
  // applying non-blocking
  fcntl(sfd, F_SETFL, O_NONBLOCK);
  _switchEpollRegisration( &_srv_hold, EPOLLIN | EPOLLHUP);
  // adding to epoll queu
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sfd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// switch epoll regitration
void Server::_switchEpollRegisration(t_epollhold *eh, uint32_t ev)
{
  _epoll_event.data.ptr = eh;
  _epoll_event.events = ev;
}

void Server::newConnection(ServerConfig &sc)
{
  //std::cout << "new connection on socket: " << sc.getFd() << std::endl;
  int client_fd = accept(sc.getFd(), NULL, NULL);
  DEBUG_INFO("------------New Request-----------");
  if (client_fd < 0)
  {
    DEBUG_ERROR("accept() failed.");
    return ;
  }
  // std::cout << "accepted client_fd: " << client_fd << " from server_fd: " << sc.getFd() << std::endl;
  // adding to clinet list
  this->_addClient(client_fd, sc);
}

bool Server::readHeaders(t_epollhold *eh)
{
  std::string headers = eh->cl->readHeaders();
  if (headers.empty())
    return false;
  try {
    DEBUG_INFO("Request");
    eh->cl->getReq().requestParser(headers);
    if (eh->cl->getReq().isCGI())
    {
      // in case of CGI i'm upgrading the Client class to CGI by using copy constructor
      CGIClient* cgi = _clients.updateToCGI(eh->cl->getFd());
      epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, cgi->getFd(), NULL);
      cgi->preSetup();
      return true;
    }
  } 
  catch (const RequestException &e){
    std::cerr << ERROR_MSG << "parsing headers: "<< e.what() << std::endl;
    // send bad request
    eh->cl->callError(e.status());
  }
  catch (const std::exception &e){
    std::cerr << ERROR_MSG << "parsing headers: "<< e.what() << std::endl;
    // send bad request
    eh->cl->callError(400);
  }
  eh->cl->preSetup();
  return false;
}

void Server::_initSocket(ServerConfig &sc) {
  // AF_INET: address family ipv4
  // SOCK_STREAM: socket type: stream-based (as opposed to SOCK_DGRAM for UDP)
  // IPPROTO_TCP protocol: TCP 
  int _socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_socket_fd < 0)
    throw ServerException("socket() failed.");
  sc.setSocketFd(_socket_fd);
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
  // bro look to man 
  _addr.sin_port = htons(sc.getPort());
  _addr.sin_addr.s_addr = inet_addr(sc.getHost().c_str());
  // convert from string to 32bit format
  if (bind(_socket_fd, (struct sockaddr *)(&_addr), sizeof(_addr)))
    // bind is like setsockopt its just tell the os that the socket belongs
    // to this address and this port
    throw ServerException("bind() failed with port" + to_string98(sc.getPort()));
  // listen()  marks  the  socket referred to by sockfd as a passive socket,
  // that is, as a socket that will be used to  accept  incoming  connection
  // requests using accept(2).
  if (listen(_socket_fd, MAX_CONX_QUEUE))
    throw ServerException("listen() failed.");
  // register on epoll
  this->_addSocketToEpoll(sc.getServHold(), _socket_fd);
  std::cout << INFO_MSG 
            << "listen on http://" 
            << inet_ntoa(_addr.sin_addr) 
            << ":" 
            << to_string98(ntohs(_addr.sin_port)) 
            << std::endl;
}


void Server::serverFree()
{
  if (_epoll_fd > -1)
    close(_epoll_fd);
  for (unsigned long i = 0; i < _servers.size(); i++)
  {
    if (_servers[i].getFd() > -1)
      close (_servers[i].getFd());
  }
}
