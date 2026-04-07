
#include "../includes/Server.hpp"
#include "../../includes/container.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

Server::Server()
{
  _port = 8080;
  _ip = "127.0.0.1";
}

Server::~Server()
{
  close(_socket_fd);
}

void Server::run()
{
  this->_initSocket();
  socklen_t size_socket = sizeof(_addr);
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
  {
    throw ServerException("socket() failed.");
  }
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
  int client_fd = accept(_socket_fd, (struct sockaddr *)(&_addr), (socklen_t *)&size_socket);
  DEBUG_INFO("------------New Request-----------");
  if (client_fd < 0)
    throw ServerException("accept() failed.");


  // read from user client socket
  // read to the line before \r\n\r\n 
  std::string buffer;
  char tmp[BUF_SIZE];
  size_t pos = 0;
  size_t tmppos;
  while (1)
  {
    int bytes = recv(client_fd, tmp, BUF_SIZE, 0);
    if (bytes <= 0)
      break;
    buffer.append(tmp, bytes);
    tmppos = buffer.find("\r\n\r\n");
    if ( tmppos != std::string::npos)
    {
      pos = tmppos; 
      break;
    }
  }

  std::string headers = buffer.substr(0, pos);
  std::string body    = buffer.substr(pos + 4);

  DEBUG_INFO("Request");
  Request req(headers);

  // TODO: if method is post, we should first read body
  //
  DEBUG_INFO("Response");
  Response res(req);

  // TODO:in case of get we should count file size and it CHUNK_SIZE
  if (req.getMethod() == "get"){}

  std::string result = res.build();
  // write back to user fd
  write(client_fd, result.c_str(), result.length());
  // close
  close(client_fd);
}
